//
// Created by Danya Smelskiy on 12.02.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

#include "Server.h"
#include "Utils.h"

#define BACKLOG 100

void PrintHelp(FILE* output) {
    fprintf(output, "Program requires exactly two input arguments in the following order:\n"
                    "1. Working directory path;\n"
                    "2. Listening 'address:port';\n");
}

void VerifyInputDirectory(const char* input_dir) {
    struct stat stats;
    if (stat(input_dir, &stats) < 0) {
        perror("Input directory is invalid");
        PrintHelp(stderr);
        exit(errno);
    }
    if (!(stats.st_mode & S_IFDIR)) {
        fprintf(stderr, "Input path '%s' doesn't correspond to the directory\n", input_dir);
        PrintHelp(stderr);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Wrong number of input arguments.\n");
        PrintHelp(stderr);
        return 1;
    }

    const char* input_dir = argv[1];
    VerifyInputDirectory(input_dir);

    char* address = NULL;
    uint32_t port = -1;
    ParseAddressAndPort(argv[2], &address, &port);

    printf("Input directory: %s\n", input_dir);
    printf("Address: %s\n", address);
    printf("Port: %u\n", port);

    ServerArgs server_args;
    server_args.directory_path = input_dir;
    RunServer(address, port, BACKLOG, &server_args);

    free(address);
    return 0;
}
