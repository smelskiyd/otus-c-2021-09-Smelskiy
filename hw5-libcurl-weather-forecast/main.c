//
// Created by Danya Smelskiy on 22.12.2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ReadURLData.h"
#include "jansson.h"

const char* kWeatherURL = "https://www.metaweather.com/api/";

void PrintWeatherInfo(MemoryStruct* chunk) {
    printf("%s\n", chunk->memory);
}

int main(int argc, char** argv) {
    curl_global_init(CURL_GLOBAL_ALL);

    MemoryStruct* chunk = NULL;
    chunk = ReadURLData(kWeatherURL);

    if (chunk == NULL) {
        fprintf(stderr, "Failed to read data from URL");
        curl_global_cleanup();
        return 1;
    }

    printf("Weather data was read successfully\n");

    PrintWeatherInfo(chunk);

    free(chunk->memory);
    free(chunk);
    curl_global_cleanup();
}
