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

const char* kTelnetProtocolStr = "telnet";
const char* kTelehackName = "telehack.com";

char buffer[MAX_BUFFER_LEN];

char* CombineInputWords(char** words, int n) {
    if (n < 0) {
        return NULL;
    }

    size_t total_length = 0;
    total_length += n ? n - 1 : 0; // whitespaces
    for (int i = 0; i < n; ++i) {
        total_length += strlen(words[i]);
    }

    char* result = (char*)malloc(total_length + 1);
    size_t last_pos = 0;
    for (int i = 0; i < n; ++i) {
        if (i) {
            result[last_pos++] = ' ';
        }
        snprintf(result + last_pos, total_length + 1 - last_pos, "%s", words[i]);
        last_pos += strlen(words[i]);
    }
    result[last_pos] = '\0';

    return result;
}

void GetTelehackAddr(struct sockaddr** addr, socklen_t* addr_len, int sock_type) {
    struct addrinfo* info = NULL;
    if (getaddrinfo(kTelehackName, kTelnetProtocolStr, NULL, &info)) {
        perror("Failed to get Telehack address info");
        exit(errno);
    }

    struct addrinfo* found_addr = info;

    while (found_addr && found_addr->ai_socktype != sock_type) {
        found_addr = info->ai_next;
    }

    if (!found_addr) {
        *addr = NULL;
        *addr_len = 0;
        freeaddrinfo(info);
        return;
    }

    *addr = (struct sockaddr*)(malloc(sizeof(struct sockaddr)));
    **addr = *found_addr->ai_addr;
    *addr_len = found_addr->ai_addrlen;
    freeaddrinfo(info);
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
    if (argc < 3) {
        fprintf(stderr, "Wrong number of input arguments.\n"
                        "Program requires 2 input arguments in the following order:\n"
                        "- text font;\n"
                        "- input text (it might be separated in individual words);\n");
        return 1;
    }

    const char* font = argv[1];
    char* input_text = CombineInputWords(argv + 2, argc - 2);

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

    free(addr);
    free(input_text);
    return 0;
}
