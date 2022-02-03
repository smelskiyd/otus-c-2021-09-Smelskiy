//
// Created by Danya Smelskiy on 30.01.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>

#include "TelnetProtocol.h"

#define MAX_BUFFER_LEN 4000

const char* kFigletStr = "figlet";

const char* kTelehackName = "telehack.com";

char buffer[MAX_BUFFER_LEN + 1];

void GetTelehackAddr(int sock_type, struct sockaddr** addr, socklen_t* addr_len) {
    struct addrinfo* info = NULL;
    if (getaddrinfo(kTelehackName, "telnet", NULL, &info)) {
        perror("Failed to get Telehack address info");
        exit(errno);
    }

    while (info && info->ai_socktype != sock_type) {
        info = info->ai_next;
    }

    if (!info) {
        *addr = NULL;
        *addr_len = 0;
        return;
    }

    *addr = info->ai_addr;
    *addr_len = info->ai_addrlen;
}

void ReadStartMessage(int fd) {
    buffer[0] = '\0';

    do {
        int len = (int)recv(fd, buffer, MAX_BUFFER_LEN, 0);
        buffer[len] = '\0';
        if (len < 0) {
            perror("Failed to receive data from server");
            exit(errno);
        }
        if (len == 0) {
            fprintf(stderr, "Connection was closed");
            exit(EXIT_FAILURE);
        }
        if (len > 2 && buffer[len - 1] == '.' && buffer[len - 2] == '\n') {
            break;
        }
    } while (1);

    printf("Successfully read start message from server\n\n");
}

void SendFigletCommand(int fd, const char* font, const char* text) {
    const size_t command_str_len = strlen(kFigletStr) + 2 + strlen(font) + 1 + strlen(text) + 1;
    char* command_str = (char*)(malloc(command_str_len));
    snprintf(command_str, command_str_len, "%s %c%s %s", kFigletStr, '/', font, text);
    printf("Command: \"%s\"\n", command_str);

    const unsigned char command_prefix[] = {IAC, WILL, ECHO_C, IAC, DONT, ECHO_C,
                                            IAC, WILL, SUPPRESS, IAC, DONT, SUPPRESS,
                                            13, 10, '\0'};
    const unsigned char command_suffix[] = {13, 10, '\0'};

    snprintf(buffer, MAX_BUFFER_LEN, "%s%s%s", command_prefix, command_str, command_suffix);

    size_t cmd_len = strlen(buffer);

    int len = (int)send(fd, buffer, cmd_len + 1, 0);
    if (len < 0) {
        perror("Failed to send data to server");
        exit(errno);
    }

    printf("Successfully sent the command to the server\n");

    free(command_str);
}

void ReadResultResponse(int fd) {
    do {
        int len = (int)recv(fd, buffer, MAX_BUFFER_LEN, 0);
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

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Wrong number of input arguments.\n"
                        "Program requires exactly 2 input arguments:\n"
                        "- text font;\n"
                        "- input text;\n");
        return 1;
    }

    const char* font = argv[1];
    const char* input_text = argv[2];

    printf("Font: \"%s\"\n", font);
    printf("Input text: \"%s\"\n", input_text);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("Failed to create a socket");
        exit(errno);
    }

    struct sockaddr* addr = NULL;
    socklen_t addr_len = 0;
    GetTelehackAddr(SOCK_STREAM, &addr, &addr_len);
    if (!addr) {
        printf("Failed to find TCP address of the endpoint\n");
        return 1;
    }

    if (connect(fd, addr, addr_len)) {
        perror("Failed to connect to server\n");
        exit(errno);
    }
    printf("\nSuccessfully connected to server\n");

    ReadStartMessage(fd);

    SendFigletCommand(fd, font, input_text);

    ReadResultResponse(fd);

    return 0;
}
