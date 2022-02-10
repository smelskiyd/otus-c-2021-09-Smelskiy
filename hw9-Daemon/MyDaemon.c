//
// Created by Danya Smelskiy on 13.01.2022.
//

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#include "CommonData.h"
#include "FileInfoMonitoring.h"

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

/*
 * Daemonize current program
 */
void Daemonize() {
    /*
     * Reset initial process mask
     */
    umask(0);

    /*
     * First fork
     */
    int fork1_status = fork();
    if (fork1_status < 0) {
        perror("Failed to do first fork");
        exit(1);
    } else if (fork1_status != 0) {
        exit(0);
    }

    /*
     * setsid: create new session
     */
    pid_t setsid_status = setsid();
    if (setsid_status < 0) {
        syslog(LOG_CRIT, "Failed to create new session");
        exit(1);
    }

    /*
     * Guarantee the managing terminal can't be changed
     */
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        syslog(LOG_CRIT, "Failed to ignore signal SIGHUP");
        exit(1);
    }

    /*
     * Second fork
     */
    int fork2_status = fork();
    if (fork2_status < 0) {
        syslog(LOG_CRIT, "Failed to do second fork");
        exit(1);
    } else if (fork2_status != 0) {
        exit(0);
    }

    /*
     * Change current working directory
     */
    if (chdir("/") < 0) {
        syslog(LOG_CRIT, "Failed to change current working directory");
        exit(1);
    }

    /*
     * Close all file descriptors
     */
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        syslog(LOG_CRIT, "Failed to get maximum descriptor number");
    }
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (rlim_t i = 0; i < rl.rlim_max; i++) {
        close((int)i);
    }

    /*
     * Set stdin/stdout/stderr file descriptors to /dev/null
     */
    int fd0 = open("/dev/null", O_RDWR);
    int fd1 = dup(0);
    int fd2 = dup(0);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_CRIT, "Wrong file descriptors %d %d %d", fd0, fd1, fd2);
        exit(1);
    }

    syslog(LOG_INFO, "MyDaemon was successfully daemonized");
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Wrong number of input arguments.\n"
                        "Program requires exactly one argument: path to the input file for monitoring");
        return 1;
    }

    InitializeLogging(argv[0]);

    syslog(LOG_INFO, "MyDaemon has started");

    int server_fd = ConnectToServer();

    const char* file_path = argv[1];
    StartMonitoring(file_path, server_fd);

    close(server_fd);
//    Daemonize();
  //  sleep(100);
}
