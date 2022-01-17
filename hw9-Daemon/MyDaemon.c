//
// Created by Danya Smelskiy on 13.01.2022.
//

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <syslog.h>

/*
 * Daemonize current program
 */
void Daemonize(const char* program_name) {
    /*
     * Initialize logging
     */
    openlog(program_name, LOG_CONS, LOG_DAEMON);

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
    Daemonize(argv[0]);
    sleep(100);
}
