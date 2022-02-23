//
// Created by daniilsmelskiy on 20.01.2022.
//

#include <stdio.h>
#include <stdlib.h>
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
    printf("Successfully created a socket.\n");

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, kSocketPath);
    size_t addr_length = sizeof(addr.sun_family) + strlen(kSocketPath);

    unlink(kSocketPath);
    int bind_status = bind(fd, (struct sockaddr*)&addr, addr_length);
    if (bind_status < 0) {
        close(fd);
        perror("Failed to bind address to socket");
        return errno;
    }
    printf("Successfully binded address.\n");

    int listen_status = listen(fd, 1);
    if (listen_status < 0) {
        close(fd);
        perror("Failed to start listening the socket");
        return errno;
    }
    printf("Successfully started to listen input connections.\n");

    char buffer[MAX_MSG_LENGTH];

    do {
        printf("Waiting for the client to connect...\n");

        int wd = accept(fd, NULL, NULL);
        if (wd < 0) {
            close(fd);
            perror("Failed to accept the client connection");
            return errno;
        }
        printf("Client was successfully connected with file descriptor = %d.\n", wd);

        do {
            int bytes_received = recv(wd, buffer, sizeof(buffer), 0);
            if (bytes_received < 0) {
                close(wd);
                close(fd);
                perror("Failed to receive the message from client");
                return errno;
            }

            if (bytes_received == 0) {
                printf("Connection to client was closed.\n");
                break;
            }

            int bytes_read = 0;
            while (bytes_read < bytes_received) {
                if (bytes_read + (int)sizeof(ssize_t) > bytes_received) {
                    fprintf(stderr, "Received message in incorrect format.\n");
                    return EXIT_FAILURE;
                }

                const ssize_t* file_size = (const ssize_t*)&buffer[bytes_read];
                printf("File size has changed to `%ld` bytes.\n", *file_size);
                bytes_read += sizeof(file_size_t);
            }
        } while (1);

        close(wd);
    } while(1);

    close(fd);
    return 0;
}
