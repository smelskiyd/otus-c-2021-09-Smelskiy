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
#include <errno.h>

#include "CommonData.h"
#include "FileInfoMonitoring.h"

#define LOCKFILE "/tmp/MyDaemon.pid"
#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

static int PIDFileFD = -1;

void UpdatePIDFile() {
    if (PIDFileFD < 0) {
        fprintf(stderr, "Failed to update PID file: PID file is undefined\n");
        exit(EXIT_FAILURE);
    }
    ftruncate(PIDFileFD, 0);

    char buf[16];
    sprintf(buf, "%ld", (long)getpid());
    if (write(PIDFileFD, buf, strlen(buf) + 1) < 0) {
        syslog(LOG_INFO, "Failed to write to PID file");
        exit(EXIT_FAILURE);
    }
}

void SignalSIGHUPHandler(int signo) {
    if (signo == SIGHUP) {
        if (PIDFileFD != -1) {
            UpdatePIDFile();
        }
    }
}

int LockFile(int fd) {
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return fcntl(fd, F_SETLK, &fl);
}

void AlreadyRunning() {
    int fd = open(LOCKFILE, O_RDWR | O_CREAT, LOCKMODE);
    if (fd < 0) {
        perror("Failed to open PID File");
        exit(errno);
    }
    if (LockFile(fd) < 0) {
        if (errno == EACCES || errno == EAGAIN) {
            close(fd);
            fprintf(stderr, "MyDaemon is already running\n");
            exit(EXIT_FAILURE);
        }
        perror("Failed to lock PID File");
        exit(EXIT_FAILURE);
    }
    PIDFileFD = fd;

    UpdatePIDFile();
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
    if (rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024;
    }
    for (rlim_t i = 0; i < rl.rlim_max; i++) {
        if ((int)i != PIDFileFD) {
            close((int)i);
        }
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

    UpdatePIDFile();

    syslog(LOG_INFO, "MyDaemon was successfully daemonized (PID=%d)", getpid());
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Wrong number of input arguments.\n"
                        "Program requires exactly one argument: path to the input file for monitoring");
        return 1;
    }

    AlreadyRunning();

    signal(SIGHUP, SignalSIGHUPHandler);

    InitializeLogging(argv[0]);

    syslog(LOG_INFO, "MyDaemon has started");

    int server_fd = ConnectToServer();

    Daemonize();

    const char* file_path = argv[1];
    StartMonitoring(file_path, server_fd);

    close(PIDFileFD);
    close(server_fd);
    return 0;
}
