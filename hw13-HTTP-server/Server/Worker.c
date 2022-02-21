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
#include <fcntl.h>

#include "HTTP.h"
#include "Utils.h"

ssize_t GetFileSize(const char* file_path) {
    struct stat stats;
    if (stat(file_path, &stats) < 0) {
        fprintf(stderr, "Failed to get file size: `%s`\n", file_path);
        return -1;
    }
    return stats.st_size;
}

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
    snprintf(response, MAX_RESPONSE_LEN, "HTTP/1.0 %d %s\r\n\r\n", status_code, status_message);

    if (Send(fd, response, strlen(response)) < 0) {
        perror("Failed to send an error response to client");
        return;
    }

    printf("Successfully sent an error response to client %d: status code = %d, status message = `%s`\n",
           fd, status_code, status_message);
}

int SendFile(int sfd, const char* file_path) {
    printf("Sending file response to client %d\n", sfd);

    ssize_t file_size = GetFileSize(file_path);
    if (file_size < 0) {
        SendError(sfd, 400, "Bad request");
        return -1;
    }

    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        SendError(sfd, 400, "Bad request");
        return -1;
    }

    char response_header[MAX_RESPONSE_LEN];
    snprintf(response_header, MAX_RESPONSE_LEN, "HTTP/1.0 200 OK\r\n"
                                                "Content-Length: %ld\r\n\r\n", file_size);
    if (Send(sfd, response_header, strlen(response_header)) < 0) {
        perror("Failed to send a response to client");
        close(fd);
        return -1;
    }

    long long len = 0;
    for (off_t offset = 0; offset < file_size; offset += len) {
        ssize_t status = sendfile(fd, sfd, offset, &len, NULL, 0);
        if (status < 0) {
            perror("Failed to send a file to server");
            close(fd);
            return -1;
        }
        if (status == 0) {
            // file has reached the end
            break;
        }
    }

    close(fd);
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

static void WorkerCleanup(void* args) {
    int* finished_flag = (int*)args;
    *finished_flag = 1;
}

void* ProcessConnection(void* args) {
    WorkerArgs* worker_args = (WorkerArgs*)args;
    int fd = worker_args->fd;
    const char* directory_path = worker_args->directory_path;
    int* finish_flag = worker_args->finish_flag;
    pthread_cleanup_push(WorkerCleanup, (void*)finish_flag);
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

    pthread_cleanup_pop(1);
    pthread_exit(NULL);
}

typedef struct {
    pthread_t thread;
    int worker_inited;
    int worker_finished;
} WorkerInfo;

static WorkerInfo* workers;
static size_t workers_count = 0;

void InitWorkers(size_t n) {
    workers_count = n;
    workers = (WorkerInfo*)malloc(sizeof(WorkerInfo) * n);
    for (size_t i = 0; i < n; ++i) {
        workers[i].worker_inited = 0;
        workers[i].worker_finished = 1;
    }
}

void DestroyWorkers() {
    for (size_t i = 0; i < workers_count; ++i) {
        if (workers[i].worker_inited) {
            pthread_join(workers[i].thread, NULL);
        }
    }
    free(workers);
    workers_count = 0;
}

WorkerInfo* FindFreeWorker() {
    for (size_t i = 0; i < workers_count; ++i) {
        if (workers[i].worker_finished) {
            if (workers[i].worker_inited) {
                // It's possible that thread is almost gone but isn't finished yet
                pthread_join(workers[i].thread, NULL);
            }
            return &workers[i];
        }
    }
    return NULL;
}

void RunWorker(int fd, const char* directory_path) {
    if (workers == NULL) {
        fprintf(stderr, "Workers are not inited.\n");
        return;
    }

    WorkerInfo* worker = NULL;
    while ((worker = FindFreeWorker()) == NULL) {
    }
    if (worker == NULL) {
        fprintf(stderr, "Failed to find free worker\n");
        return;
    }

    printf("Running new worker for client %d\n", fd);

    worker->worker_inited = 1;
    worker->worker_finished = 0;

    WorkerArgs* worker_args = (WorkerArgs*)(malloc(sizeof(WorkerArgs)));
    worker_args->fd = fd;
    worker_args->directory_path = directory_path;
    worker_args->finish_flag = &worker->worker_finished;

    if (pthread_create(&worker->thread, NULL, ProcessConnection, (void*)worker_args)) {
        fprintf(stderr, "Failed to create thread");
        exit(1);
    }
}
