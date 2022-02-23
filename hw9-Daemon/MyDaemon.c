//
// Created by Danya Smelskiy on 13.01.2022.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#include "CommonData.h"
#include "Daemonizer.h"
#include "FileInfoMonitoring.h"

/*
 * Print help info (how to start the program)
 */
void PrintHelp(FILE* output) {
    fprintf(output, "Program requires at least one argument in the following order:\n"
                    "1. Path to the input file for monitoring;\n"
                    "2. -d option to daemonize the program;\n");
}

/*
 * Initialize logging
 */
void InitializeLogging(const char* program_name) {
    openlog(program_name, LOG_CONS, LOG_DAEMON);
}

/*
 * Connect to server
 */
int ConnectToServer() {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        syslog(LOG_CRIT, "Failed to create a socket.");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, kSocketPath, strlen(kSocketPath));
    size_t addr_length = sizeof(addr.sun_family) + strlen(kSocketPath);

    int status = connect(fd, (struct sockaddr*)&addr, addr_length);
    if (status < 0) {
        close(fd);
        syslog(LOG_CRIT, "Failed to connect to server.");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "Successfully connected to server.");

    return fd;
}

int main(int argc, char** argv) {
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Wrong number of input arguments.\n");
        PrintHelp(stderr);
        return 1;
    }

    int daemonize = 0;
    if (argc == 3) {
        if (strcmp(argv[2], "-d") != 0) {
            fprintf(stderr, "Invalid format of program arguments.\n");
            PrintHelp(stderr);
            return 1;
        }
        daemonize = 1;
    }

    InitializeLogging(argv[0]);

    if (daemonize) {
        Daemonize();
        syslog(LOG_INFO, "MyDaemon has started in daemon mode on PID: %d", getpid());
    } else {
        syslog(LOG_INFO, "MyDaemon has started in simple mode on PID: %d", getpid());
    }

    int server_fd = ConnectToServer();
    const char* file_path = argv[1];

    StartMonitoring(file_path, server_fd);

    close(PIDFileFD);
    close(server_fd);
    return 0;
}
