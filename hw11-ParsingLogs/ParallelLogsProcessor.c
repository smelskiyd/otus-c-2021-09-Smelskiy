//
// Created by Danya Smelskiy on 30.01.2022.
//

#include "ParallelLogsProcessor.h"

LogsProcessorResult* RunParallelProcessing(size_t n_threads, FileWithMutex* files, size_t files_n) {
    // Init threads results
    LogsProcessorResult* processors_results = NULL;
    processors_results = (LogsProcessorResult*)malloc(sizeof(LogsProcessorResult) * n_threads);
    for (size_t i = 0; i < n_threads; ++i) {
        InitLogsProcessorResult(&processors_results[i]);
    }

    // Create threads
    pthread_t* threads = (pthread_t*)(malloc(sizeof(pthread_t) * n_threads));
    for (size_t i = 0; i < n_threads; ++i) {
        ThreadArgs* thread_args = (ThreadArgs*)(malloc(sizeof(ThreadArgs)));
        thread_args->processor_result = &processors_results[i];
        thread_args->files = files;
        thread_args->files_n = files_n;
        thread_args->thread_id = i;

        if (pthread_create(&threads[i], NULL, SingleThreadProcess, (void*)thread_args)) {
            fprintf(stderr, "Failed to create thread");
            exit(1);
        }
    }

    // Wait for all threads to complete
    for (size_t i = 0; i < n_threads; ++i) {
        pthread_join(threads[i], NULL);
        printf("Thread %zu has processed %zu logs\n", i, processors_results[i].total_logs_processed);
    }
    free(threads);

    // Combine(merge) all results
    LogsProcessorResult* total_result = (LogsProcessorResult*)(malloc(sizeof(LogsProcessorResult)));
    InitLogsProcessorResult(total_result);
    for (size_t i = 0; i < n_threads; ++i) {
        CombineTwoResults(total_result, &processors_results[i]);
        DestroyLogsProcessorResult(&processors_results[i]);
    }
    free(processors_results);

    return total_result;
}

LogsProcessorResult* RunParallelLogsProcessor(FileNode* head, size_t files_n, size_t n_threads) {
    // Create FileWithMutex structure for each file
    FileWithMutex* files = (FileWithMutex*)(malloc(sizeof(FileWithMutex) * files_n));
    for (size_t i = 0; i < files_n; ++i, head = head->next) {
        FileWithMutex* cur = &files[i];
        FileWithMutexInit(cur, head->file_path);
    }

    LogsProcessorResult* result = RunParallelProcessing(n_threads, files, files_n);

    // Destruct all FileWithMutex's
    for (size_t i = 0; i < files_n; ++i) {
        FileWithMutex* cur = &files[i];
        FileWithMutexDestroy(cur);
    }
    free(files);

    return result;
}
