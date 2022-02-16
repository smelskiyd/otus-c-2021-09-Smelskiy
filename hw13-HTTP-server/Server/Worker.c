//
// Created by Danya Smelskiy on 17.02.2022.
//

#include "Worker.h"

#include <sys/socket.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "HTTP.h"
#include "Utils.h"

#define MAX_BUFFER_LEN 4096

int Send(int sfd, const char* message, size_t len) {
    ssize_t bytes_sent;
    for (size_t i = 0; i < len; i += bytes_sent) {
        bytes_sent = send(sfd, message + i, len - i, 0);
        if (bytes_sent < 0) {
            return -1;
        }
    }
    return 0;
}

void SendError(int fd, int status_code, const char* status_message) {
    printf("Sending error response to client %d\n", fd);

    char response[MAX_RESPONSE_LEN];
    snprintf(response, MAX_RESPONSE_LEN, "HTTP/1.1 %d %s\n", status_code, status_message);

    if (Send(fd, response, strlen(response)) < 0) {
        perror("Failed to send an error response to client");
        return;
    }

    printf("Successfully sent an error response to client %d: status code = %d, status message = `%s`\n",
           fd, status_code, status_message);
}

int SendFile(int fd, const char* file_path) {
    printf("Sending file response to client %d\n", fd);

    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        SendError(fd, 400, "Bad request");
        return -1;
    }

    const char* response_header = "HTTP/1.1 200 OK\n\n";
    if (Send(fd, response_header, strlen(response_header)) < 0) {
        perror("Failed to send a response to client");
        fclose(file);
        return -1;
    }

    char buffer[MAX_BUFFER_LEN + 1];
    while (!feof(file)) {
        size_t len = fread(buffer, 1, MAX_BUFFER_LEN, file);
        if (Send(fd, buffer, len) < 0) {
            perror("Failed to send a response to client");
            fclose(file);
            return -1;
        }
    }

    fclose(file);
    return 0;
}

int VerifyFileRequest(const char* file_path, int* status_code, const char** status_message) {
    struct stat stats;
    if (stat(file_path, &stats) < 0) {
        *status_code = 404;
        *status_message = "Not Found";
        return -1;
    }
    if (!(stats.st_mode & R_OK)) {
        *status_code = 403;
        *status_message = "Forbidden";
        return -1;
    }
    if ((stats.st_mode & S_IFMT) != S_IFREG) {
        *status_code = 400;
        *status_message = "Bad request: Path doesn't correspond to regular file";
        return -1;
    }
    *status_code = 200;
    *status_message = "OK";
    return 0;
}

void ProcessRequest(int fd, char* request_str, const char* directory_path) {
    Request* request = ParseRequest(request_str);

    if (request == NULL) {
        SendError(fd, 400, "Bad request");
        return;
    }

    printf("Received a request from client %d: command=`%s`, file name=`%s`, http version=`%s`\n",
           fd, request->command, request->file_name, request->http_version);

    int status_code;
    const char* status_message;
    if (VerifyRequest(request, &status_code, &status_message) < 0) {
        SendError(fd, status_code, status_message);
        free(request);
        return;
    }

    char* file_path = ConcatenateStrings(directory_path, request->file_name);
    if (VerifyFileRequest(file_path, &status_code, &status_message) < 0) {
        SendError(fd, status_code, status_message);
        free(request);
        free(file_path);
        return;
    }

    if (SendFile(fd, file_path) < 0) {
        fprintf(stderr, "Failed to sent a response to client %d\n", fd);
    } else {
        printf("Successfully sent a response to client %d\n", fd);
    }
    free(file_path);
    free(request);
}

void* ProcessConnection(void* args) {
    pthread_detach(pthread_self());

    WorkerArgs* worker_args = (WorkerArgs*)args;
    int fd = worker_args->fd;
    const char* directory_path = worker_args->directory_path;
    free(worker_args);

    char request[MAX_REQUEST_LEN];

    ssize_t len = recv(fd, request, MAX_REQUEST_LEN, 0);

    if (len < 0) {
        if (errno & (ECONNREFUSED | EBADF)) {
            fprintf(stderr, "Failed to receive message from client %d. Client has disconnected.\n", fd);
            close(fd);
            pthread_exit(NULL);
        }
        close(fd);
        perror("Failed to receive a message from client");
        pthread_exit(NULL);
    }

    if (len > 0) {
        request[len] = '\0';
        ProcessRequest(fd, request, directory_path);
    }

    close(fd);
    pthread_exit(NULL);
}

void RunWorker(WorkerArgs* args) {
    printf("Running new worker for client %d\n", args->fd);

    pthread_t* thread = (pthread_t*)(malloc(sizeof(pthread_t)));
    if (pthread_create(thread, NULL, ProcessConnection, (void*)args)) {
        fprintf(stderr, "Failed to create thread");
        exit(1);
    }
    // TODO(@smelskiyd): fix memory leak
}
