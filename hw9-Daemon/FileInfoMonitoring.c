//
// Created by daniilsmelskiy on 18.01.2022.
//

#include "FileInfoMonitoring.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>

#include "CommonData.h"

file_size_t GetFileSize(const char* file_path) {
    struct stat file_stat;
    if (stat(file_path, &file_stat) != 0) {
        syslog(LOG_CRIT, "Failed to get file stats for file `%s`. Errno: %d", file_path, errno);
    }
    return (file_size_t)file_stat.st_size;
}

void SendMessageToServer(int server_fd, file_size_t current_file_size) {
    static char buffer[MAX_MSG_LENGTH];
    memcpy(buffer, (void*)&current_file_size, sizeof(current_file_size));

    int length = sizeof(current_file_size);
    int bytes_sent = send(server_fd, buffer, length, 0);

    if (bytes_sent < 0) {
        close(server_fd);
        syslog(LOG_CRIT, "Failed to send message to server.");
        exit(EXIT_FAILURE);
    }

    if (bytes_sent != length) {
        close(server_fd);
        syslog(LOG_CRIT, "Last message wasn't sent completely. Disconnecting...");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "Successfully sent %d bytes to server.\n", length);
}

void StartMonitoring(const char* file_path, int server_fd) {
    int fd = inotify_init();
    if (fd < 0) {
        close(server_fd);
        syslog(LOG_CRIT, "Failed to initialize inotify.");
        exit(EXIT_FAILURE);
    }

    int wd = inotify_add_watch(fd, file_path, (IN_DELETE | IN_MODIFY));
    if (wd == -1) {
        close(server_fd);
        syslog(LOG_CRIT, "Failed to add watch to inotify.");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "Started monitoring the file: `%s`", file_path);

    char buffer[MAX_MSG_LENGTH];

    int len;
    do {
        len = read(fd, buffer, MAX_MSG_LENGTH);
        if (len < 0) {
            close(server_fd);
            syslog(LOG_CRIT, "Failed to read data from inotify file descriptor. Errno: ", errno);
            exit(EXIT_FAILURE);
        }

        int pos = 0;
        while (pos < len) {
            struct inotify_event* event = (struct inotify_event*)(&buffer[pos]);

            if (event->mask & (IN_MODIFY | IN_DELETE)) {
                syslog(LOG_INFO, "File '%s' was changed, new file size is = %lld", file_path, GetFileSize(file_path));

                SendMessageToServer(server_fd, GetFileSize(file_path));
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
