//
// Created by Danya Smelskiy on 17.02.2022.
//

#include "Server.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "Worker.h"

#define MAX_EVENTS 128
#define MAX_WORKERS 128

int SetNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int CreateServer(const char* address, uint32_t port, int backlog) {
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) {
        perror("Failed to create a socket");
        exit(errno);
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address);
    if (bind(sfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Failed to bind an address to a socket");
        close(sfd);
        exit(errno);
    }

    if (listen(sfd, backlog) < 0) {
        perror("Failed to start listening socket");
        close(sfd);
        exit(errno);
    }

    printf("Address is successfully binded to socket.\n");

    if (SetNonBlocking(sfd) == -1) {
        perror("Failed to set non blocking mode on socket file descriptor");
        close(sfd);
        exit(errno);
    }

    printf("Server is successfully created.\n");

    signal(SIGPIPE, SIG_IGN);

    return sfd;
}

void RegisterEvent(const int kqueue_id, int fd, int filter, int flags) {
    struct kevent event;
    EV_SET(&event, fd, filter, flags, 0, 0, 0);

    if (kevent(kqueue_id, &event, 1, NULL, 0, NULL) < 0) {
        perror("Failed to register new kevent");
        exit(1);
    }
}

void RunServer(const char* address, uint32_t port, int backlog, ServerArgs* args) {
    int sfd = CreateServer(address, port, backlog);
    printf("Server has started.\n");

    int kqueue_id = kqueue();
    if (kqueue_id < 0) {
        perror("Failed to create kqueue");
        close(sfd);
        exit(EXIT_FAILURE);
    }

    RegisterEvent(kqueue_id, sfd, EVFILT_READ, EV_ADD | EV_ENABLE);

    struct kevent events[MAX_EVENTS];

    InitWorkers(MAX_WORKERS);

    do {
        int events_n = kevent(kqueue_id, NULL, 0, events, MAX_EVENTS, NULL);
        if (events_n < 0) {
            perror("Failed to check new events with kevent");
            close(kqueue_id);
            close(sfd);
            exit(1);
        }

        for (int i = 0; i < events_n; ++i) {
            int event_fd = (int)events[i].ident;

            if (events[i].flags & EV_EOF) {
                printf("Client with file descriptor '%d' has disconnected\n", event_fd);
                close(event_fd);
                continue;
            }

            if (event_fd == sfd) {
                printf("Found new client connection\n");

                int new_client_fd = accept(sfd, NULL, NULL);
                if (new_client_fd < 0) {
                    perror("Failed to accept new connection");
                    close(kqueue_id);
                    close(sfd);
                    exit(EXIT_FAILURE);
                }

                if (SetNonBlocking(new_client_fd) == -1) {
                    perror("Failed to set non blocking mode on client connection file descriptor");
                    close(kqueue_id);
                    close(new_client_fd);
                    close(sfd);
                    exit(errno);
                }

                RegisterEvent(kqueue_id, new_client_fd, EVFILT_READ, EV_ADD | EV_ONESHOT);
                continue;
            }

            if (events[i].filter & EVFILT_READ) {
                RunWorker(event_fd, args->directory_path);
                continue;
            }
        }
    } while (1);

    DestroyWorkers();
    close(kqueue_id);
    close(sfd);
}
