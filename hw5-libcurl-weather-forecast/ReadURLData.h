//
// Created by Danya Smelskiy on 29.12.2021.
//

#ifndef OTUSHW_READURLDATA_H
#define OTUSHW_READURLDATA_H

#include "curl/curl.h"

typedef struct {
    char* memory;
    size_t size;
} MemoryStruct;

MemoryStruct* CreateMemoryStruct(size_t size);

CURL* CreateCurlHandler();

MemoryStruct* ReadURLData(const char* url);

#endif //OTUSHW_READURLDATA_H
