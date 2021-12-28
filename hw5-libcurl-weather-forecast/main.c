//
// Created by Danya Smelskiy on 22.12.2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "curl/curl.h"
#include "jansson.h"

const char* kWeatherURL = "https://www.metaweather.com/api/";

typedef struct {
    char* memory;
    size_t size;
} MemoryStruct;

MemoryStruct* CreateMemoryStruct(size_t size) {
    MemoryStruct* memory = (MemoryStruct*)(malloc(sizeof(MemoryStruct)));
    if (memory == NULL) {
        return NULL;
    }
    memory->size = size;
    memory->memory = (char*)(malloc(size));
    return memory;
}

static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void *userp) {
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

void SetInputURL(CURL* curl_handle, const char* input_url) {
    curl_easy_setopt(curl_handle, CURLOPT_URL, input_url);
}

MemoryStruct* ReadWeatherData(CURL* curl_handle) {
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    MemoryStruct* chunk = NULL;
    chunk = CreateMemoryStruct(1);
    if (chunk == NULL || chunk->memory == NULL) {
        return chunk;
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

    return chunk;
}

int main(int argc, char** argv) {
    curl_global_init(CURL_GLOBAL_ALL);

    CURL* curl_handle = NULL;
    curl_handle = CreateCurlHandler();
    if (curl_handle == NULL) {
        fprintf(stderr, "Failed to create curl handler");
        curl_global_cleanup();
        return 1;
    }

    SetInputURL(curl_handle, kWeatherURL);

    MemoryStruct* chunk = NULL;
    chunk = ReadWeatherData(curl_handle);

    if (chunk == NULL) {
        fprintf(stderr, "Failed to read data from URL");
        curl_easy_cleanup(curl_handle);
        curl_global_cleanup();
        return 1;
    }

    printf("Weather data was read successfully\n");

    free(chunk->memory);
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
}
