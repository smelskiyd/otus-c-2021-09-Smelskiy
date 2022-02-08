//
// Created by daniilsmelskiy on 20.01.2022.
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
    printf("Successfully created a socket.\n");

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);
    size_t addr_length = sizeof(addr.sun_family) + strlen(SOCKET_PATH);

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

            printf("New message received: %s\n", buffer);
        } while (1);

        close(wd);
    } while(1);

    close(fd);
    return 0;
}
