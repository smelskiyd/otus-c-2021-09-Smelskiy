//
// Created by Danya Smelskiy on 19.02.2022.
//

#ifndef OTUSHW_HTTP_H
#define OTUSHW_HTTP_H

#include <sys/types.h>

#define MAX_REQUEST_LEN 512
#define MAX_RESPONSE_LEN 4096

typedef struct {
    char* command;
    char* file_name;
    char* http_version;
} Request;

void RequestInit(Request* request);

Request* ParseRequest(char* request_str);
int VerifyRequest(const Request* request, int* status_code, const char** status_message);

typedef struct {
    char* http_version;
    char* status_code;
    char* status_message;

    ssize_t content_length;
} Response;

void ResponseInit(Response* response);

Response* ParseResponse(char* response_str);
void ParseHeader(char* header, Response* response);

#endif //OTUSHW_HTTP_H
