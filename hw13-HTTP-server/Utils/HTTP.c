//
// Created by Danya Smelskiy on 19.02.2022.
//

#include "HTTP.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void RequestInit(Request* request) {
    request->command = NULL;
    request->file_name = NULL;
    request->http_version = NULL;
}

Request* ParseRequest(char* request_str) {
    Request* request = (Request*)malloc(sizeof(Request));
    RequestInit(request);

    request->command = strtok(request_str, " ");
    request->file_name = strtok(NULL, " ");
    request->http_version = strtok(NULL, " \n\r");

    if (request->command == NULL || request->file_name == NULL || request->http_version == NULL) {
        free(request);
        return NULL;
    }

    return request;
}

int VerifyRequest(const Request* request, int* status_code, const char** status_message) {
    if (strcmp(request->command, "GET") != 0) {
        *status_code = 400;
        *status_message = "Bad request: server supports only GET requests";
        return -1;
    }

    if (request->file_name[0] != '/') {
        *status_code = 400;
        *status_message = "Bad request: file name must start with '/'";
        return -1;
    }

    if (strcmp(request->http_version, "HTTP/1.0") != 0) {
        *status_code = 400;
        *status_message = "Bad request: server supports only requests with HTTP/1.0 version";
        return -1;
    }

    *status_code = 200;
    *status_message = "OK";
    return 0;
}

void ResponseInit(Response* response) {
    response->http_version = NULL;
    response->status_code = NULL;
    response->status_message = NULL;
}

Response* ParseResponse(char* response_str) {
    Response* response = (Response*)malloc(sizeof(Response));
    ResponseInit(response);

    response->http_version = strtok(response_str, " ");
    response->status_code = strtok(NULL, " ");
    response->status_message = strtok(NULL, "\n\r");

    if (response->http_version == NULL || response->status_code == NULL || response->status_message == NULL) {
        free(response);
        return NULL;
    }

    return response;
}

void ParseHeader(char* header, Response* response) {
    char* header_name = strtok(header, ":\r\n");
    if (strcmp(header_name, "Content-Length") == 0) {
        char* content_length = strtok(NULL, "\r\n");
        if (content_length == NULL) {
            fprintf(stderr, "Failed to parse content-length header\n");
            response->content_length = -1;
            return;
        }
        response->content_length = strtol(content_length, NULL, 10);
    } else {
        fprintf(stderr, "Failed to parse header from HTTP response. Undefined header name: `%s`\n", header_name);
    }
}
