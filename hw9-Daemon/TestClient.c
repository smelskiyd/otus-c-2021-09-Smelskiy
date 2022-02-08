//
// Created by daniilsmelskiy on 21.01.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/mydaemon_socket"

int main(int argc, char** argv) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        fprintf(stderr, "Failed to create socket\n");
        exit(1);
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);
    size_t addr_length = sizeof(addr.sun_family) + strlen(SOCKET_PATH);

    int status = connect(fd, (struct sockaddr*)&addr, addr_length);
    if (status < 0) {
        close(fd);
        fprintf(stderr, "Failed to connect to socket\n");
        exit(1);
    }

    printf("Successfully connected to server\n");

    char buffer[256];
    while (1) {
        int length = sprintf(buffer, "%llu\n", 1ull);
        if (send(fd, buffer, length + 1, 0) < 0) {
            fprintf(stderr, "Failed to send message. Disconnecting...\n");
            break;
        }
        printf("Successfully sent %d bytes\n", length);

        sleep(10);
    }

    close(fd);
    return 0;
}
