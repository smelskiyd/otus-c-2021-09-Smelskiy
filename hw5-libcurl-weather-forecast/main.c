//
// Created by Danya Smelskiy on 22.12.2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ReadURLData.h"
#include "JSONVisualizer.h"

// Weather-Forecast API URL
const char* kWeatherAPIURL = "https://www.metaweather.com/api/location/";

void PrintWeatherInfo(const char* weather_data) {
    json_t* root = NULL;
    json_error_t error;
    root = json_loads(weather_data, 0, &error);
    if (root) {
        RecursivelyPrintJson(root, 0);
        json_decref(root);
    } else {
        fprintf(stderr, "Failed to read JSON file. Error on line %d: %s", error.line, error.text);
        exit(1);
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Wrong number of input arguments. "
                        "Program requires exactly 1 input argument: "
                        "location name (e.g. 'moscow', 'london', etc)");
        exit(1);
    }
    const char* location = argv[1];
    printf("Location: %s\n", location);

    curl_global_init(CURL_GLOBAL_ALL);

    MemoryStruct* chunk = NULL;
    chunk = ReadURLData(kWeatherAPIURL);

    if (chunk == NULL) {
        fprintf(stderr, "Failed to read data from URL");
        curl_global_cleanup();
        return 1;
    }

    printf("Weather data was read successfully\n");

    PrintWeatherInfo(chunk->memory);

    free(chunk->memory);
    free(chunk);
    curl_global_cleanup();
}
