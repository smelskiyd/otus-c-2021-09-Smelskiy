//
// Created by daniilsmelskiy on 23.02.2022.
//

#include "Daemonizer.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/stat.h>

#define LOCKFILE "/tmp/MyDaemon.pid"
#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

static int PIDFileFD = -1;

void UpdatePIDFile() {
    if (PIDFileFD < 0) {
        syslog(LOG_ERR, "Failed to update PID file: PID file is undefined");
        return;
    }
    if (ftruncate(PIDFileFD, 0) < 0) {
        syslog(LOG_ERR, "Failed to truncate PID file");
        return;
    }

    char buf[16];
    sprintf(buf, "%ld", (long)getpid());
    if (write(PIDFileFD, buf, strlen(buf) + 1) < 0) {
        syslog(LOG_ERR, "Failed to write to PID file");
        return;
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

void LockPIDFile() {
    int fd = open(LOCKFILE, (O_RDWR | O_CREAT), LOCKMODE);
    if (fd < 0) {
        syslog(LOG_CRIT, "Failed to open PID File");
        exit(EXIT_FAILURE);
    }
    if (LockFile(fd) < 0) {
        if (errno == EACCES || errno == EAGAIN) {
            syslog(LOG_CRIT, "MyDaemon is already running\n");
        } else {
            syslog(LOG_CRIT, "Failed to lock PID File");
        }
        exit(EXIT_FAILURE);
    }
    PIDFileFD = fd;

    UpdatePIDFile();

    syslog(LOG_INFO, "Successfully locked PID file");
}

/*
 * SIGHUP Handler: updates PID File
 */
void SignalSIGHUPHandler(int signal_id) {
    if (signal_id == SIGHUP) {
        if (PIDFileFD != -1) {
            UpdatePIDFile();
        }
    }
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
     * Update PID file on SIGHUP
     */
    signal(SIGHUP, SignalSIGHUPHandler);

    /*
     * First fork
     */
    int fork1_status = fork();
    if (fork1_status < 0) {
        perror("Failed to do first fork");
        exit(EXIT_FAILURE);
    } else if (fork1_status != 0) {
        exit(EXIT_SUCCESS);
    }

    /*
     * setsid: create new session
     */
    pid_t setsid_status = setsid();
    if (setsid_status < 0) {
        syslog(LOG_CRIT, "Failed to create new session");
        exit(EXIT_FAILURE);
    }

    /*
     * Second fork
     */
    int fork2_status = fork();
    if (fork2_status < 0) {
        syslog(LOG_CRIT, "Failed to do second fork while daemonizing");
        exit(EXIT_FAILURE);
    } else if (fork2_status != 0) {
        exit(EXIT_SUCCESS);
    }

    /*
     * Change current working directory
     */
    if (chdir("/") < 0) {
        syslog(LOG_CRIT, "Failed to change current working directory");
        exit(EXIT_FAILURE);
    }

    /*
     * Close all file descriptors (except PID file descriptor)
     */
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        syslog(LOG_CRIT, "Failed to get maximum descriptor number");
        exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
    }

    /*
     * Lock PID file and verify that daemon isn't already running
     */
    LockPIDFile();
}
