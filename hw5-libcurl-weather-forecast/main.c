//
// Created by Danya Smelskiy on 22.12.2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ReadURLData.h"
#include "JSONVisualizer.h"

// Weather-Forecast API URL
// Append WOEID of the location to get weather forecast URL for this location
// For instance: "https://www.metaweather.com/api/location/44418"
const char* kWeatherAPIURL = "https://www.metaweather.com/api/location/";

// Get location info for string name of the location
// For instance: "https://www.metaweather.com/api/location/search/?query=london"
// It is used to find location WOEID
const char* kLocationInfoURL = "https://www.metaweather.com/api/location/search/?query=";

char* AppendWOEIDToURL(const char* url, int woeid) {
    char* num;
    asprintf(&num, "%d", woeid);

    char* result = malloc(strlen(url) + strlen(num) + 2);
    *result = '\0';

    strncat(result, url, strlen(url));
    strncat(result, num, strlen(num));

    // Append '\' to the end of the URL
    result[strlen(url) + strlen(num)] = '/';
    // Append null character to the end of the C-string
    result[strlen(url) + strlen(num) + 1] = '\0';

    free(num);
    return result;
}

// Parse JSON object to get WOEID value
int GetWOEIDFromJson(json_t* root) {
    if (!json_is_array(root)) {
        return -1;
    }
    if (json_array_size(root) != 1) {
        return -1;
    }
    json_t* root_object = json_array_get(root, 0);
    if (!json_is_object(root_object)) {
        return -1;
    }
    json_t* woeid_json_value = json_object_get(root_object, "woeid");
    if (!woeid_json_value) {
        return -1;
    }
    if (!json_is_integer(woeid_json_value)) {
        return -1;
    }
    return (int)json_integer_value(woeid_json_value);
}

void PrintWeatherInfo(const char* weather_data) {
    json_t* root = NULL;
    json_error_t error;
    root = json_loads(weather_data, 0, &error);
    if (root) {
        printf("Weather info:\n");
        RecursivelyPrintJson(root, 0);
        printf("\n");
        json_decref(root);
    } else {
        fprintf(stderr, "Failed to read JSON file. Error on line %d: %s", error.line, error.text);
        exit(1);
    }
}

// Download location info from URL in JSON format
int FindWOEID(const char* location) {
    char buffer[256] = {'\0'};
    snprintf(buffer, strlen(kLocationInfoURL) + 1, "%s", kLocationInfoURL);
    snprintf(buffer + strlen(kLocationInfoURL), strlen(location) + 1, "%s", location);
    buffer[strlen(kLocationInfoURL) + strlen(location)] = '\0';

    MemoryStruct* chunk = NULL;
    chunk = ReadURLData(buffer);

    if (chunk == NULL || chunk->memory == NULL) {
        fprintf(stderr, "Failed to read data from %s\n", buffer);
        if (chunk) {
            free(chunk);
        }
        curl_global_cleanup();
        exit(1);
    }

    json_t* root = NULL;
    json_error_t error;
    root = json_loads(chunk->memory, 0, &error);

    free(chunk->memory);
    free(chunk);

    if (root) {
        int woeid = GetWOEIDFromJson(root);
        if (woeid == -1) {
            fprintf(stderr, "Failed to get WOEID of %s, from %s\n", location, buffer);
            exit(1);
        }
        printf("WOEID of %s = %d\n", location, woeid);

        json_decref(root);
        return woeid;
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

    int woeid = FindWOEID(location);
    char* url_with_woeid = AppendWOEIDToURL(kWeatherAPIURL, woeid);

    MemoryStruct* chunk = NULL;
    chunk = ReadURLData(url_with_woeid);

    if (chunk == NULL) {
        fprintf(stderr, "Failed to read data from URL");
        curl_global_cleanup();
        return 1;
    }

    PrintWeatherInfo(chunk->memory);

    free(chunk->memory);
    free(chunk);
    curl_global_cleanup();
}
