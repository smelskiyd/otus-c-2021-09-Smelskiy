//
// Created by Danya Smelskiy on 29.01.2022.
//

#ifndef OTUSHW_LOGSPROCESSOR_H
#define OTUSHW_LOGSPROCESSOR_H

#include "FileWithMutex.h"
#include "HashMap.h"

typedef struct {
    size_t total_logs_processed;
    size_t total_bytes_send;
    HashMap* url_counter;
    HashMap* referers_counter;
} LogsStatistics;

void InitLogsStatistics(LogsStatistics* result);
void DestroyLogsStatistics(LogsStatistics* result);

/*
 * Merge two instances of LogsStatistics and save the result in 'fir'.
 * In other words all data from second instance is added to the first one.
 * @param fir First instance of LogsStatistics
 * @param sec Second instance of LogsStatistics
 */
void CombineTwoLogsStatistics(LogsStatistics* fir, const LogsStatistics* sec);

typedef struct {
    LogsStatistics* processor_results;
    FileWithMutex* files;
    size_t files_n;
    size_t thread_id;
} ThreadArgs;

void* SingleThreadProcess(void* arguments);

#endif //OTUSHW_LOGSPROCESSOR_H
