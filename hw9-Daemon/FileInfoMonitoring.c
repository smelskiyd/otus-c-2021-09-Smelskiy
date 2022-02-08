//
// Created by daniilsmelskiy on 18.01.2022.
//

#include "FileInfoMonitoring.h"

#include <unistd.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <syslog.h>
#include <errno.h>

#define MAX_BUFFER_LENGTH 256

long long GetFileSize(const char* file_path) {
    struct stat file_stat;
    if (stat(file_path, &file_stat) != 0) {
        syslog(LOG_CRIT, "Failed to get file stats for file `%s`. Errno: %d", file_path, errno);
    }
    return (long long)file_stat.st_size;
}

void StartMonitoring(const char* file_path) {
    int fd = inotify_init();
    if (fd < 0) {
        syslog(LOG_CRIT, "Failed to initialize inotify.");
    }

    int wd = inotify_add_watch(fd, file_path, (IN_DELETE | IN_MODIFY));
    if (wd == -1) {
        syslog(LOG_CRIT, "Failed to add watch to inotify.");
    }

    syslog(LOG_INFO, "Started monitoring the file: `%s`", file_path);

    char buffer[MAX_BUFFER_LENGTH];

    int len;
    do {
        len = read(fd, buffer, MAX_BUFFER_LENGTH);
        if (len < 0) {
            syslog(LOG_CRIT, "Failed to read data from inotify file descriptor. Errno: ", errno);
        }

        int pos = 0;
        while (pos < len) {
            struct inotify_event* event = (struct inotify_event*)(&buffer[pos]);

            if (event->mask & (IN_MODIFY | IN_DELETE)) {
                syslog(LOG_INFO, "File '%s' was changed, new file size is = %lld", file_path, GetFileSize(file_path));
            } else {
                syslog(LOG_WARNING, "Undefined event from inotify, mask = %d", event->mask);
            }

            if (event->len) {
                pos += event->len;
            }
            pos += sizeof(struct inotify_event);
        }
    } while (len >= 0);

    inotify_rm_watch(fd, wd);
    close(fd);
}
