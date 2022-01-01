//
// Created by Danya Smelskiy on 31.12.2021.
//

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "MyLogger.h"

#ifndef MAX_HEADER_LOG_LENGTH
    #define MAX_HEADER_LOG_LENGTH 256
#endif

static const LogLevel kDefaultLogLevel = LEVEL_DEBUG;
static LogLevel global_log_level = kDefaultLogLevel;

// NULL represents a default output stream: stderr for ERROR and FATAL levels, and stdout for others
static FILE* output_log_file = NULL;

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

char* get_log_header(const char* file_name, int line) {
    static char buf[MAX_HEADER_LOG_LENGTH];
    sprintf(buf, "%s, %d: ", file_name, line);
    return buf;
}

void print_log(const char* file_name, int line,
               const LogLevel log_level, const char* const log_format, ...) {
    if (!is_enabled_log_level(log_level)) {
        return;
    }

    char* header = NULL;
    header = get_log_header(file_name, line);
    if (header == NULL) {
        return;
    }
    size_t header_length = strlen(header);

    size_t format_length = strlen(log_format);
    size_t log_length = header_length + format_length + 1;

    char* format = NULL;
    format = (char*)malloc(log_length);
    if (format == NULL) {
        fprintf(stderr, "Failed to allocate memory for log with length %zu\n", log_length);
        return;
    }

    strncpy(format, header, header_length);
    strncpy(format + header_length, log_format, strlen(log_format));

    va_list arg_list;
    va_start(arg_list, log_format);
    FILE* output_file = get_output_log_file(log_level);
    if (vfprintf(output_file, format, arg_list) < 0) {
        fprintf(stderr, "Failed to write log to output file\n");
    }
    va_end(arg_list);

    free(format);
}
