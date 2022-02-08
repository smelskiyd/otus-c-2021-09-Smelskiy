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
    strcpy(addr.sun_path, SOCKET_PATH);
    size_t addr_length = sizeof(addr.sun_family) + strlen(SOCKET_PATH);

    int status = connect(fd, (struct sockaddr*)&addr, addr_length);
    if (status < 0) {
        close(fd);
        perror("Failed to connect to server");
        return errno;
    }

    printf("Successfully connected to server.\n");

    char buffer[MAX_MSG_LENGTH];
    do {
        sprintf(buffer, "%d", 123);
        int length = strlen(buffer);
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
        printf("Successfully sent %d bytes to server. Message = `%s`.\n", length, buffer);

        sleep(10);
    } while (1);

    close(fd);
    return 0;
}
