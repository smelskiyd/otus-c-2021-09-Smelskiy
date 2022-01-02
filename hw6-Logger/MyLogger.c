//
// Created by Danya Smelskiy on 31.12.2021.
//

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <execinfo.h>

#include "MyLogger.h"

#ifndef MAX_HEADER_LOG_LENGTH
    #define MAX_HEADER_LOG_LENGTH 256
#endif

#ifndef MAX_BACKTRACE_SIZE
    #define MAX_BACKTRACE_SIZE 100
#endif

static const LogLevel kDefaultLogLevel = LEVEL_DEBUG;
static LogLevel global_log_level = kDefaultLogLevel;

// NULL represents a default output stream: stderr for ERROR and FATAL levels, and stdout for others
static FILE* output_log_file = NULL;

char* convert_level_to_string(LogLevel level) {
    switch (level) {
        case LEVEL_TRACE:
            return "TRACE";
        case LEVEL_DEBUG:
            return "DEBUG";
        case LEVEL_INFO:
            return "INFO";
        case LEVEL_WARNING:
            return "WARNING";
        case LEVEL_ERROR:
            return "ERROR";
        case LEVEL_FATAL:
            return "FATAL";
        case LEVEL_OFF:
            return "OFF";
        default:
            fprintf(stderr, "Failed to convert LogLevel to string: %d\n", (int)level);
            return "UNDEFINED";
    }
}

void set_global_log_level(const LogLevel log_level) {
    global_log_level = log_level;
}

LogLevel get_global_log_level() {
    return global_log_level;
}

bool is_enabled_log_level(const LogLevel log_level) {
    return log_level != LEVEL_OFF &&
           (int)log_level >= (int)get_global_log_level();
}

void set_output_log_file(FILE* output_file) {
    output_log_file = output_file;
}

void reset_output_log_file() {
    output_log_file = NULL;
}

FILE* get_output_log_file(const LogLevel log_level) {
    if (output_log_file == NULL) {
        if (log_level == LEVEL_ERROR || log_level == LEVEL_FATAL) {
            return stderr;
        }
        return stdout;
    }
    return output_log_file;
}

char* get_log_header(const char* file_name, int line, const LogLevel log_level) {
    static char buf[MAX_HEADER_LOG_LENGTH];
    sprintf(buf, "%s: %s, %d: ", convert_level_to_string(log_level), file_name, line);
    return buf;
}

char* get_format_string(const char* file_name, int line, const LogLevel log_level,
                        const char* const log_format) {
    char* header = NULL;
    header = get_log_header(file_name, line, log_level);
    if (header == NULL) {
        return NULL;
    }
    size_t header_length = strlen(header);

    size_t format_length = strlen(log_format);
    size_t log_length = header_length + format_length + 2;

    char* format = NULL;
    format = (char*)malloc(log_length);
    if (format == NULL) {
        fprintf(stderr, "Failed to allocate memory for log with length %zu\n", log_length);
        return NULL;
    }

    strncpy(format, header, header_length);
    strncpy(format + header_length, log_format, strlen(log_format));
    format[header_length + format_length] = '\n';
    format[log_length - 1] = '\0';

    return format;
}

void print_log(const char* file_name, int line,
               const LogLevel log_level, const char* const log_format, ...) {
    if (!is_enabled_log_level(log_level)) {
        return;
    }

    char* format_string = get_format_string(file_name, line, log_level, log_format);

    va_list arg_list;
    va_start(arg_list, log_format);
    FILE* output_file = get_output_log_file(log_level);
    if (vfprintf(output_file, format_string, arg_list) < 0) {
        fprintf(stderr, "Failed to write log to output file\n");
    }
    va_end(arg_list);

    free(format_string);
}

#if defined(__unix__) || defined(__APPLE__)
void print_backtrace(FILE* output_file) {
    void* buffer[MAX_BACKTRACE_SIZE];
    int size = backtrace(buffer, MAX_BACKTRACE_SIZE);
    char** buffer_symbols = backtrace_symbols(buffer, size);
    if (buffer_symbols == NULL) {
        perror("backtrace_symbols");
        exit(1);
    }

    for (int i = 0; i < size; ++i) {
        fprintf(output_file, "%s\n", buffer_symbols[i]);
    }
    free(buffer_symbols);
}
#else
void print_backtrace(FILE* output_file) {
    #warning "Backtrace isn't implemented for this operating system"
}
#endif

void print_fatal_log(const char* file_name, int line,
                     const LogLevel log_level, const char* const log_format, ...) {
    if (!is_enabled_log_level(log_level)) {
        return;
    }

    char* format_string = get_format_string(file_name, line, log_level, log_format);

    va_list arg_list;
    va_start(arg_list, log_format);
    FILE* output_file = get_output_log_file(log_level);
    if (vfprintf(output_file, format_string, arg_list) < 0) {
        fprintf(stderr, "Failed to write log to output file\n");
    }
    va_end(arg_list);

    print_backtrace(output_file);

    free(format_string);

    exit(1);
}
