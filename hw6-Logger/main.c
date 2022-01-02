//
// Created by Danya Smelskiy on 31.12.2021.
//

#include <stdio.h>
#include <errno.h>

#include "MyLogger.h"

int main(int argc, char** argv) {
    FILE* output_log_file;
    output_log_file = fopen("./tmp_file", "wb");
    if (output_log_file == NULL) {
        perror("Can't open the input file");
        return errno;
    }

    set_output_log_file(output_log_file);
    PRINT_LOG_INFO("%s", "info log");

    reset_output_log_file();

    PRINT_LOG_WARNING("%s", "warning log");
    PRINT_LOG_ERROR("%s", "error log");
    PRINT_LOG_FATAL("%s", "fatal log");
}
