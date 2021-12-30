//
// Created by Danya Smelskiy on 29.12.2021.
//

#include <stdlib.h>
#include <string.h>

#include "ReadURLData.h"

MemoryStruct* CreateMemoryStruct(size_t size) {
    MemoryStruct* memory = (MemoryStruct*)(malloc(sizeof(MemoryStruct)));
    if (memory == NULL) {
        return NULL;
    }
    memory->size = size;
    // Size + 1 terminal null symbol
    memory->memory = (char*)(malloc(size + 1));
    return memory;
}

size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t real_size = size * nmemb;
    MemoryStruct* mem = (MemoryStruct*)userp;

    char* ptr = realloc(mem->memory, mem->size + real_size + 1);
    if (!ptr) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, real_size);
    mem->size += real_size;
    mem->memory[mem->size] = 0;

    return real_size;
}

CURL* CreateCurlHandler() {
    CURL* curl_handle = NULL;

    curl_handle = curl_easy_init();
    if (curl_handle == NULL) {
        return NULL;
    }

    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    return curl_handle;
}

MemoryStruct* ReadURLData(const char* url) {
    CURL* curl_handle = NULL;
    curl_handle = CreateCurlHandler();

    if (curl_handle == NULL) {
        fprintf(stderr, "Failed to create curl handler");
        exit(1);
    }

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    MemoryStruct* chunk = NULL;
    chunk = CreateMemoryStruct(0);

    if (chunk == NULL || chunk->memory == NULL) {
        fprintf(stderr, "Failed to allocate memory data");
        if (chunk != NULL) {
            free(chunk);
        }
        curl_easy_cleanup(curl_handle);
        exit(1);
    }

    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)chunk);

    CURLcode res;
    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        exit(1);
    } else {
        printf("%lu bytes retrieved\n", (unsigned long)chunk->size);
    }

    curl_easy_cleanup(curl_handle);
    return chunk;
}
