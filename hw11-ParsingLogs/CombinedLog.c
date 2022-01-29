//
// Created by Danya Smelskiy on 28.01.2022.
//

#include "CombinedLog.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// p - pointer to the pointer to the first not processed character
char* ReadUntil(char** p, char stop_symbol, int skip_first) {
    if (p == NULL || (*p == NULL)) {
        return NULL;
    }

    while (skip_first--) {
        if (**p == '\0') {
            return NULL;
        }
        (*p)++;
    }

    size_t len = 0;
    {
        char* pp = *p;
        while ((*pp != '\0') && (*pp != stop_symbol)) {
            ++len;
            pp++;
        }
    }

    char* result = (char*)(malloc((len + 1) * sizeof(char)));

    for (size_t i = 0; i < len; ++i, ++(*p)) {
        result[i] = (**p);
    }
    result[len] = '\0';

    return result;
}

void InitCombinedLog(CombinedLog* log) {
    log->ip = NULL;
    log->l = NULL;
    log->user = NULL;
    log->time = NULL;
    log->request_line = NULL;
    log->status = NULL;
    log->return_size = NULL;
    log->referer = NULL;
    log->user_agent = NULL;
}

void FreeCombinedLog(CombinedLog* log) {
    free(log->ip);
    free(log->l);
    free(log->user);
    free(log->time);
    free(log->request_line);
    free(log->status);
    free(log->return_size);
    free(log->referer);
    free(log->user_agent);
    free(log);
}

CombinedLog* ParseCombinedLog(char* str) {
    CombinedLog* result = NULL;
    result = (CombinedLog*)malloc(sizeof(CombinedLog));
    InitCombinedLog(result);

    char* p = str;

    result->ip = ReadUntil(&p, ' ', 0);
    result->l = ReadUntil(&p, ' ', 1);
    result->user = ReadUntil(&p, ' ', 1);
    result->time = ReadUntil(&p, ']', 2);
    result->request_line = ReadUntil(&p, '"', 3);
    result->status = ReadUntil(&p, ' ', 2);
    result->return_size = ReadUntil(&p, ' ', 1);
    result->referer = ReadUntil(&p, '\"', 2);
    result->user_agent = ReadUntil(&p, '\"', 3);

    return result;
}

long long StringToInteger(const char* str) {
    if (str == NULL) {
        fprintf(stderr, "Failed to convert string to integer: empty string");
        exit(0);
    }
    long long result = 0;
    result = strtol(str, NULL, 0);
    return result;
}

void PrintCombinedLog(const CombinedLog* log) {
    printf("{");
    printf("ip: %s", log->ip ? log->ip : "-");
    printf("; l: %s", log->l ? log->l : "-");
    printf("; user: %s", log->user ? log->user : "-");
    printf("; time: %s", log->time ? log->time : "-");
    printf("; request_line: \"%s\"", log->request_line ? log->request_line : "-");
    printf("; status: %s", log->status ? log->status : "-");
    printf("; return_size: %s", log->return_size ? log->return_size : "-");
    printf("; referer: \"%s\"", log->referer ? log->referer : "-");
    printf("; user_agent: \"%s\"", log->user_agent ? log->user_agent : "-");
    printf("}\n");
}
