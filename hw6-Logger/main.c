//
// Created by Danya Smelskiy on 31.12.2021.
//

#include <stdio.h>

#include "MyLogger.h"

int main(int argc, char** argv) {
    FILE* output_file;
    output_file = fopen("./tmp_file", "wb");
    set_output_log_file(output_file);
    PRINT_LOG_INFO("%s", "info log");
    reset_output_log_file();
    PRINT_LOG_WARNING("%s", "warning log");
    PRINT_LOG_ERROR("%s", "error log");
}
