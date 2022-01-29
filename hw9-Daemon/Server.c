//
// Created by daniilsmelskiy on 20.01.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/mydaemon_socket"
#define MAX_NUMBER_OF_CLIENTS 1

int main(int argc, char** argv) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        fprintf(stderr, "Failed to create socket\n");
        exit(1);
    }
    printf("Successfully created socket\n");

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);
    size_t addr_length = sizeof(addr.sun_family) + strlen(SOCKET_PATH);
    int bind_status = bind(fd, (struct sockaddr*)&addr, addr_length);
    if (bind_status < 0) {
        close(fd);
        fprintf(stderr, "Failed to bind address\n");
        exit(2);
    }
    printf("Successfully binded address\n");

    int listen_status = listen(fd, MAX_NUMBER_OF_CLIENTS);
    if (listen_status < 0) {
        close(fd);
        fprintf(stderr, "Failed to start listening the socket\n");
        exit(3);
    }
    printf("Successfully started listening\n");

    printf("Waiting for any connections...\n");

    char buffer[1024];

    while (1) {
        printf("Waiting for any connections...\n");
        int wd = accept(fd, NULL, NULL);
        if (wd < 0) {
            fprintf(stderr, "Failed to accept the incoming connection\n");
            break;
        }

        printf("Successfully connected to client\n");

        while (1) {
            int length = 0;
            if ((length = recv(wd, buffer, sizeof(buffer), 0)) < 0) {
                fprintf(stderr, "Failed to receive data\n");
                break;
            }

            if (length == 0) {
                continue;
            }

            printf("New message received: %s\n", buffer);
        }

        close(wd);
    }

    close(fd);
    return 0;
}
