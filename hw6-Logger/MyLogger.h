//
// Created by Danya Smelskiy on 31.12.2021.
//

#ifndef OTUSHW_MYLOGGER_H
#define OTUSHW_MYLOGGER_H

#include <stdbool.h>

typedef enum {
    LEVEL_TRACE = 0,
    LEVEL_DEBUG,
    LEVEL_INFO,
    LEVEL_WARNING,
    LEVEL_ERROR,
    LEVEL_FATAL,
    LEVEL_OFF
} LogLevel;

/*
 * Set logging level
 */
void set_global_log_level(LogLevel log_level);

/*
 * Get current logging level
 */
LogLevel get_global_log_level();

/*
 * Check whether any logging level is enabled
 */
bool is_enabled_log_level(LogLevel log_level);

/*
 * Set the FILE where to output logs
 */
void set_output_log_file(FILE* output_file);

/*
 * Reset custom output file to the default (stderr for ERROR and FATAL levels, and stdout for others)
 */
void reset_output_log_file();

/*
 * Get current output logging file
 */
FILE* get_output_log_file(LogLevel log_level);

/*
 * Print backtrace. Use MAX_BACKTRACE_SIZE compile definition to set the depth of backtrace
 */
void print_backtrace(FILE* output_file);

void print_log(const char* file_name, int line, LogLevel log_level, const char* format, ...);
void print_fatal_log(const char* file_name, int line, LogLevel log_level, const char* format, ...);

#define PRINT_LOG_TRACE(...) \
    print_log(__FILE__, __LINE__, LEVEL_TRACE, __VA_ARGS__)
#define PRINT_LOG_DEBUG(...) \
    print_log(__FILE__, __LINE__, LEVEL_DEBUG, __VA_ARGS__)
#define PRINT_LOG_INFO(...) \
    print_log(__FILE__, __LINE__, LEVEL_INFO, __VA_ARGS__)
#define PRINT_LOG_WARNING(...) \
    print_log(__FILE__, __LINE__, LEVEL_WARNING, __VA_ARGS__)
#define PRINT_LOG_ERROR(...) \
    print_log(__FILE__, __LINE__, LEVEL_ERROR, __VA_ARGS__)
#define PRINT_LOG_FATAL(...) \
    print_fatal_log(__FILE__, __LINE__, LEVEL_FATAL, __VA_ARGS__)

#endif //OTUSHW_MYLOGGER_H
