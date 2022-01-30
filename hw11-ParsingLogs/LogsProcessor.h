//
// Created by Danya Smelskiy on 29.01.2022.
//

#ifndef OTUSHW_LOGSPROCESSOR_H
#define OTUSHW_LOGSPROCESSOR_H

#include "FileWithMutex.h"
#include "HashMap.h"

// TODO: Change to 'LogsProcessorResults'
typedef struct {
    size_t total_logs_processed;
    size_t total_bytes_send;
    HashMap* url_counter;
    HashMap* referers_counter;
} LogsProcessorResult;

void InitLogsProcessorResult(LogsProcessorResult* result);
void DestroyLogsProcessorResult(LogsProcessorResult* result);

/*
 * Merges two instances of LogsProcessorResult and saves the result in 'fir'.
 * In other words all data from second instance is added to the first one.
 * @param fir First instance of LogsProcessorResult
 * @param sec Second instance of LogsProcessorResults
 */
void CombineTwoResults(LogsProcessorResult* fir, const LogsProcessorResult* sec);

typedef struct {
    LogsProcessorResult* processor_result;
    FileWithMutex* files;
    size_t files_n;
    size_t thread_id;
} ThreadArgs;

void* SingleThreadProcess(void* arguments);

#endif //OTUSHW_LOGSPROCESSOR_H
