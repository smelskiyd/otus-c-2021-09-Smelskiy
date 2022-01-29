//
// Created by Danya Smelskiy on 29.01.2022.
//

#ifndef OTUSHW_LOGSPROCESSOR_H
#define OTUSHW_LOGSPROCESSOR_H

#include "FileWithMutex.h"

typedef struct {
    size_t total_logs_processed;
    size_t total_bytes_send;
} LogsProcessorResult;

void InitLogsProcessorResult(LogsProcessorResult* result);
void CombineTwoResults(LogsProcessorResult* fir, const LogsProcessorResult* sec);

typedef struct {
    LogsProcessorResult* processor_result;
    FileWithMutex* files;
    size_t files_n;
    size_t thread_id;
} ThreadArgs;

void* SingleThreadProcess(void* arguments);

#endif //OTUSHW_LOGSPROCESSOR_H
