//
// Created by Danya Smelskiy on 30.01.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <stdbool.h>

#include "TelnetProtocol.h"

#define MAX_BUFFER_LEN 4096

const char* kFigletStr = "figlet";

const char* kTelehackName = "telehack.com";

char buffer[MAX_BUFFER_LEN];

void GetTelehackAddr(struct sockaddr** addr, socklen_t* addr_len, int sock_type) {
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

// A single dot on the new line means that the server is waiting for the response
bool ServerWaitsForNewCommand(const char* str, ssize_t len) {
    if (len <= 0) {
        return false;
    }
    if (len == 1) {
        return str[len - 1] == '.';
    }
    return str[len - 2] == '\n' && str[len - 1] == '.';
}

void ReadStartMessage(int fd) {
    do {
        ssize_t len = recv(fd, buffer, MAX_BUFFER_LEN, 0);
        if (len < 0) {
            perror("Failed to receive data from server");
            exit(errno);
        }
        if (len == 0) {
            fprintf(stderr, "Connection was closed");
            exit(EXIT_FAILURE);
        }
        // A single dot on the new line means that the server is waiting for the response
        if (ServerWaitsForNewCommand(buffer, len)) {
            break;
        }
    } while (1);

    /// Notes:
    /// Received commands: {255 251 3} {255 251 1} {255 253 24} {255 253 31}

    printf("Successfully read start message from server\n\n");
}

void SendFigletCommand(int fd, const char* font, const char* text) {
    const size_t command_str_len = strlen(kFigletStr) + 2 + strlen(font) + 1 + strlen(text) + 1;
    char* command_str = (char*)(malloc(command_str_len));
    snprintf(command_str, command_str_len, "%s %c%s %s", kFigletStr, '/', font, text);
    printf("Command: \"%s\"\n", command_str);

    const unsigned char command_prefix[] = {IAC, DONT, SUPPRESS, IAC, WILL, SUPPRESS,
                                            IAC, DONT, ECHO_C, IAC, WILL, ECHO_C,
                                            13, 10, '\0'};
    const unsigned char command_suffix[] = {13, 10, '\0'};

    snprintf(buffer, MAX_BUFFER_LEN, "%s%s%s", command_prefix, command_str, command_suffix);

    const size_t cmd_len = strlen(buffer);

    ssize_t len = send(fd, buffer, cmd_len + 1, 0);
    if (len < 0) {
        perror("Failed to send data to server");
        exit(errno);
    }
    if (len == 0) {
        fprintf(stderr, "Connection was closed");
        exit(EXIT_FAILURE);
    }

    printf("Successfully sent the command to the server\n");

    free(command_str);
}

void ReadResultResponse(int fd) {
    do {
        ssize_t len = recv(fd, buffer, MAX_BUFFER_LEN, 0);
        if (len < 0) {
            perror("Failed to receive data from server");
            exit(errno);
        }
        if (len == 0) {
            fprintf(stderr, "Connection was closed");
            exit(EXIT_FAILURE);
        }
        if (ServerWaitsForNewCommand(buffer, len)) {
            // Remove single dot at new line when server returns the result
            buffer[len - 1] = '\0';
            printf("%s", buffer);
            break;
        }
        buffer[len] = '\0';
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

    struct sockaddr* addr = NULL;
    socklen_t addr_len = 0;
    GetTelehackAddr(&addr, &addr_len, SOCK_STREAM);
    if (!addr) {
        printf("Failed to find TCP address of the endpoint\n");
        return 1;
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("Failed to create a socket");
        exit(errno);
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
