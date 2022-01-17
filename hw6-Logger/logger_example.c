//
// Created by Danya Smelskiy on 31.12.2021.
//

#include <stdio.h>
#include <stdlib.h>

#include "MyLogger.h"

void SimpleLogging() {
    PRINT_LOG_TRACE("%s", "trace log");
    PRINT_LOG_DEBUG("%s, %.3f", "debug log", 123.123f);
    PRINT_LOG_INFO("%s, %d", "info log", 123);
    PRINT_LOG_WARNING("%s", "warning log");
    PRINT_LOG_ERROR("%s", "error log");
}

void B() {
    PRINT_LOG_FATAL("%s", "fatal log");
}

void A() {
    B();
}

void LoggingToFile() {
    FILE* output_log_file;
    output_log_file = fopen("./tmp_logging_test_file", "wb");
    if (output_log_file == NULL) {
        perror("Failed to open the input file");
        exit(1);
    }
    set_output_log_file(output_log_file);

    PRINT_LOG_TRACE("%s, %d", "trace log", 123);
    PRINT_LOG_DEBUG("%s, %.3f", "debug log", 123.123f);
    PRINT_LOG_INFO("%s", "info log");
    PRINT_LOG_WARNING("%s", "warning log");
    PRINT_LOG_ERROR("%s", "error log");
    A();

    fclose(output_log_file);
}

int main(int argc, char** argv) {
    SimpleLogging();
    LoggingToFile();
    return 0;
}
