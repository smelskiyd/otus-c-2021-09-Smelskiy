//
// Created by Danya Smelskiy on 29.01.2022.
//

#include "LogsProcessor.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "CombinedLog.h"

#define LOGS_BUFFER_LIMIT 1000

char* CopyString(char* src) {
    char* result = NULL;
    result = (char*)(malloc(strlen(src) + 1));
    strncpy(result, src, strlen(src) + 1);
    return result;
}

void InitLogsProcessorResult(LogsProcessorResult* result) {
    result->total_logs_processed = 0;
    result->total_bytes_send = 0;
    result->url_counter = CreateHashMap(kDefaultHashMapSize);
    result->referers_counter = CreateHashMap(kDefaultHashMapSize);

    if (result->url_counter == NULL || result->referers_counter == NULL) {
        fprintf(stderr, "Failed to initialize LogsProcessorResult\n");
        exit(1);
    }
}

void DestroyLogsProcessorResult(LogsProcessorResult* result) {
    DestructHashMap(result->url_counter);
    DestructHashMap(result->referers_counter);
}

void CombineTwoResults(LogsProcessorResult* fir, const LogsProcessorResult* sec) {
    fir->total_logs_processed += sec->total_logs_processed;
    fir->total_bytes_send += sec->total_bytes_send;
    MergeHashMaps(fir->url_counter, sec->url_counter);
    MergeHashMaps(fir->referers_counter, sec->referers_counter);
}

// Adds info about new 'log' into processor results
void UpdateProcessStats(LogsProcessorResult* processor_result, CombinedLog* log) {
    long long return_size = StringToInteger(log->return_size);

    processor_result->total_logs_processed++;
    processor_result->total_bytes_send += return_size;

    // Increase the total weight of 'url'
    char* url = CopyString(log->request_line);
    Insert(processor_result->url_counter, url, return_size);

    // Increment the number of occurrences of 'referer'
    char* referer = CopyString(log->referer);
    Insert(processor_result->referers_counter, referer, 1);
}

/*
 * Gets new not processed lines, parses it as logs, and updates thread results
 * @param processors_result Processor results
 * @param lines_to_process Array of new (not processed) lines
 * @param lines_cnt The length of the array 'lines_to_process'
 */
void ProcessLogs(LogsProcessorResult* processor_result, char** lines_to_process, size_t lines_cnt) {
    for (size_t i = 0; i < lines_cnt; ++i) {
        char* current_line = lines_to_process[i];
        CombinedLog* log = ParseCombinedLog(current_line);
        free(current_line);

        if (log == NULL) {
            fprintf(stderr, "Failed to parse log from file\n");
            continue;
        }

        UpdateProcessStats(processor_result, log);

        FreeCombinedLog(log);
    }
}

/*
 * Read 'buffer_limit' lines from the input file, while it's owned by this thread
 * @param input_file Input file
 * @param buffer Array where to save new lines
 * @param buffer_length The length of the array 'buffer' (number of stored lines)
 * @param buffer_limit The capacity of the array 'buffer' (maximum number of lines)
 * @return Return true if file was finished after reading new lines.
 *         Return false if file isn't finished.
 */
bool ReadLogsPackage(FILE* input_file, char* buffer[], size_t* buffer_length, size_t buffer_limit) {
    bool file_has_finished = false;
    while ((*buffer_length) < buffer_limit) {
        size_t tmp;
        buffer[*buffer_length] = NULL;
        ssize_t status = getline(&buffer[*buffer_length], &tmp, input_file);
        if (status < 0) {
            file_has_finished = true;
            break;
        }
        ++(*buffer_length);
    }

    return file_has_finished;
}

void* SingleThreadProcess(void* arguments) {
    ThreadArgs* thread_args = (ThreadArgs*)arguments;
    LogsProcessorResult* processor_result = thread_args->processor_result;
    FileWithMutex* files = thread_args->files;
    size_t files_n = thread_args->files_n;
    free(thread_args);

    char* buffer[LOGS_BUFFER_LIMIT];
    size_t buffer_length = 0;
    while (true) {
        int file_id = FindReadyFileAndLock(files, files_n);
        if (file_id < 0) {
            // All files are finished
            break;
        }
        bool file_has_finished = ReadLogsPackage(files[file_id].file, buffer, &buffer_length, LOGS_BUFFER_LIMIT);
        if (file_has_finished) {
            atomic_store(&files[file_id].done, true);
        }
        pthread_mutex_unlock(files[file_id].mutex);

        ProcessLogs(processor_result, buffer, buffer_length);
        buffer_length = 0;
    }

    pthread_exit(NULL);
}
