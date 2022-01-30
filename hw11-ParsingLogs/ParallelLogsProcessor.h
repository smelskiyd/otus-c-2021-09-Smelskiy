//
// Created by Danya Smelskiy on 30.01.2022.
//

#ifndef OTUSHW_PARALLELLOGSPROCESSOR_H
#define OTUSHW_PARALLELLOGSPROCESSOR_H

#include "FileList.h"
#include "LogsProcessor.h"

LogsStatistics* RunParallelLogsProcessor(FileNode* list_of_files, size_t files_n, size_t n_threads);

#endif //OTUSHW_PARALLELLOGSPROCESSOR_H
