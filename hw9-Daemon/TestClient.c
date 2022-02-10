//
// Created by daniilsmelskiy on 21.01.2022.
//

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#include "CommonData.h"

int main(int argc, char** argv) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("Failed to create a socket");
        return errno;
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, kSocketPath, strlen(kSocketPath));
    size_t addr_length = sizeof(addr.sun_family) + strlen(kSocketPath);

    int status = connect(fd, (struct sockaddr*)&addr, addr_length);
    if (status < 0) {
        close(fd);
        perror("Failed to connect to server");
        return errno;
    }

    printf("Successfully connected to server.\n");

    char buffer[MAX_MSG_LENGTH];

    file_size_t sample_file_size = 0;

    do {
        memcpy(buffer, (void*)&sample_file_size, sizeof(file_size_t));
        int length = sizeof(file_size_t);
        int bytes_sent = send(fd, buffer, length, 0);

        if (bytes_sent < 0) {
            close(fd);
            perror("Failed to send message to server");
            return errno;
        }

        if (bytes_sent != length) {
            fprintf(stderr, "Last message wasn't sent completely. Disconnecting...\n");
            break;
        }
        printf("Successfully sent %d bytes to server.\n", length);

        ++sample_file_size;

        sleep(10);
    } while (1);

    close(fd);
    return 0;
}
