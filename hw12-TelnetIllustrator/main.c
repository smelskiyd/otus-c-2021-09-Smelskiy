//
// Created by Danya Smelskiy on 30.01.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>

const char* kTelehackName = "telehack.com";

struct sockaddr* GetTelehackAddr() {
    struct addrinfo* info = NULL;
    if (getaddrinfo(kTelehackName, "telnet", NULL, &info)) {
        perror("Failed to get Telehack address info");
        exit(errno);
    }
    printf("Family = %d\n", info->ai_family);
    printf("Sock type = %d\n", info->ai_socktype);
    printf("Protocol = %d\n", info->ai_protocol);
    struct sockaddr* addr = info->ai_addr;
    free(info);
    return addr;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Wrong number of input arguments.\n"
                        "Program requires exactly 1 input argument:\n"
                        "- Input text\n");
        return 1;
    }

    const char* input_text = argv[1];
    printf("Input text = \"%s\"\n", input_text);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("Failed to create a socket");
        exit(errno);
    }

    struct sockaddr* addr = NULL;
    addr = GetTelehackAddr();

    if (connect(fd, addr, sizeof(struct sockaddr))) {
        perror("Failed to connect to server\n");
        exit(errno);
    }

    printf("Successfully connected to server\n");

    char buffer[5000];
    do {
        int len = (int)recv(fd, buffer, 4000, 0);
        buffer[len] = '\0';
        if (len < 0) {
            perror("Failed to receive data from server");
            exit(errno);
        }
        if (len == 0) {
            break;
        }
        if (len > 2 && buffer[len - 1] == '.' && buffer[len - 2] == '\n') {
            break;
        }
    } while (1);

    printf("Successfully read everything from server\n");

    {
        const char* tmp_message = "figlet 123\n";
        int len = (int)send(fd, tmp_message, strlen(tmp_message), 0);
        if (len < 0) {
            perror("Failed to send data to server");
            exit(errno);
        }
        printf("Successfully sent %d bytes\n", len);
    }

    do {
        int len = (int)recv(fd, buffer, 4000, 0);
        buffer[len] = '\0';
        if (len < 0) {
            perror("Failed to receive data from server");
            exit(errno);
        }
        if (len == 0) {
            break;
        }
        printf("%s", buffer);
    } while (1);
}
