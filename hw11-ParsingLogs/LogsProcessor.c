//
// Created by Danya Smelskiy on 29.01.2022.
//

#include "LogsProcessor.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "CombinedLog.h"

#define LOGS_BUFFER_LIMIT 100

void InitLogsProcessorResult(LogsProcessorResult* result) {
    result->total_logs_processed = 0;
    result->total_bytes_send = 0;
}

void CombineTwoResults(LogsProcessorResult* fir, const LogsProcessorResult* sec) {
    fir->total_logs_processed += sec->total_logs_processed;
    fir->total_bytes_send += sec->total_bytes_send;
}

void UpdateProcessStats(LogsProcessorResult* processor_result, CombinedLog* log) {
    processor_result->total_logs_processed++;
    processor_result->total_bytes_send += StringToInteger(log->return_size);
}

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
