//
// Created by Danya Smelskiy on 27.01.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <limits.h>

#include "FileList.h"
#include "LogsProcessor.h"

#define TOP_N 10

FileNode* GetListOfFilesInDirectory(const char* input_dir_path) {
    DIR* dir = NULL;
    dir = opendir(input_dir_path);
    if (dir == NULL) {
        perror("Failed to open input directory");
        exit(errno);
    }

    FileNode* last_file_node = NULL;
    struct dirent* node;
    while ((node = readdir(dir)) != NULL) {
        if (node->d_type == DT_REG) {
            // TODO(@smelskiyd): Maybe need to copy
            char* full_path = (char*)(malloc(strlen(input_dir_path) + 1 + strlen(node->d_name) + 1));
            full_path = strcat(full_path, input_dir_path);
            full_path = strcat(full_path, "/");
            full_path = strcat(full_path, node->d_name);
            last_file_node = InsertNode(last_file_node, full_path);
        }
    }

    closedir(dir);

    return last_file_node;
}

void SortTopInstances(Bucket* top[], size_t length) {
    // Bubble sort
    for (size_t i = 0; i < length; ++i) {
        for (size_t j = i + 1; j < length; ++j) {
            if (top[j] != NULL) {
                // Swap, if needed
                if (top[i] == NULL) {
                    top[i] = top[j];
                    top[j] = NULL;
                } else if (top[j]->cnt > top[i]->cnt) {
                    Bucket* tmp = top[j];
                    top[j] = top[i];
                    top[i] = tmp;
                }
            }
        }
    }
}

void GetTopInstances(Bucket* top[], size_t length, HashMap* hash_map) {
    BucketsListNode* buckets;
    buckets = GetAllWords(hash_map);
    while (buckets != NULL) {
        long long min_cnt = LLONG_MAX;
        int pos = -1;
        for (size_t i = 0; i < length; ++i) {
            if (top[i] == NULL) {
                min_cnt = 0;
                pos = (int)i;
                break;
            } else if (top[i]->cnt < min_cnt) {
                min_cnt = top[i]->cnt;
                pos = (int)i;
            }
        }

        if (pos != -1) {
            Bucket* bucket_copy = (Bucket*)malloc(sizeof(Bucket));
            bucket_copy->cnt = buckets->bucket->cnt;
            char* word = buckets->bucket->word;
            bucket_copy->word = (char*)(malloc(strlen(word) + 1));
            strncpy(bucket_copy->word, word, strlen(word) + 1);

            if (top[pos] != NULL) {
                DestructBucket(top[pos]);
            }
            top[pos] = bucket_copy;
        }

        BucketsListNode* next = buckets->next;
        free(buckets);
        buckets = next;
    }

    SortTopInstances(top, length);
}

void PrintResults(const LogsProcessorResult* result) {
    printf("Program processed %zu logs\n", result->total_logs_processed);
    printf("Total number of returned bytes: %zu\n", result->total_bytes_send);

    Bucket* top_urls[TOP_N];
    for (size_t i = 0; i < TOP_N; ++i) {
        top_urls[i] = NULL;
    }
    GetTopInstances(top_urls, TOP_N, result->url_counter);
    printf("Top URLs:\n");
    for (size_t i = 0; i < TOP_N; ++i) {
        if (top_urls[i] != NULL) {
            printf("- %s [total weight = %lld]\n", top_urls[i]->word, top_urls[i]->cnt);
            DestructBucket(top_urls[i]);
        }
    }

    Bucket* top_referers[TOP_N];
    for (size_t i = 0; i < TOP_N; ++i) {
        top_referers[i] = NULL;
    }
    GetTopInstances(top_referers, TOP_N, result->referers_counter);
    printf("Top referers:\n");
    for (size_t i = 0; i < TOP_N; ++i) {
        if (top_referers[i] != NULL) {
            printf("- %s [total mentions = %lld]\n", top_referers[i]->word, top_referers[i]->cnt);
            DestructBucket(top_referers[i]);
        }
    }
}

void RunParallelProcessing(size_t n_threads, FileWithMutex* files, size_t files_n) {
    LogsProcessorResult* processors_result = NULL;
    processors_result = (LogsProcessorResult*)malloc(sizeof(LogsProcessorResult) * n_threads);
    for (size_t i = 0; i < n_threads; ++i) {
        InitLogsProcessorResult(&processors_result[i]);
    }

    pthread_t* threads = (pthread_t*)(malloc(sizeof(pthread_t) * n_threads));
    for (size_t i = 0; i < n_threads; ++i) {
        ThreadArgs* thread_args = (ThreadArgs*)(malloc(sizeof(ThreadArgs)));
        thread_args->processor_result = &processors_result[i];
        thread_args->files = files;
        thread_args->files_n = files_n;
        thread_args->thread_id = i;

        if (pthread_create(&threads[i], NULL, SingleThreadProcess, (void*)thread_args)) {
            perror("Failed to create thread");
            exit(errno);
        }
    }

    for (size_t i = 0; i < n_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
    free(threads);

    LogsProcessorResult total_result;
    InitLogsProcessorResult(&total_result);
    for (size_t i = 0; i < n_threads; ++i) {
        CombineTwoResults(&total_result, &processors_result[i]);
        printf("Thread %zu has processed %zu logs\n", i, processors_result[i].total_logs_processed);
        DestroyLogsProcessorResult(&processors_result[i]);
    }
    free(processors_result);

    PrintResults(&total_result);
}

void Process(const char* input_dir_path, size_t n_threads) {
    FileNode* list_of_files = GetListOfFilesInDirectory(input_dir_path);
    printf("Input directory has %zu files:\n", CountListSize(list_of_files));
    PrintListOfFiles(list_of_files);

    size_t files_n = CountListSize(list_of_files);
    FileWithMutex* files = (FileWithMutex*)(malloc(sizeof(FileWithMutex) * files_n));

    FileNode* head = list_of_files;
    for (size_t i = 0; i < files_n; ++i, head = head->next) {
        FileWithMutex* cur = &files[i];
        FileWithMutexInit(cur, head->file_path);
    }
    FreeList(list_of_files);

    RunParallelProcessing(n_threads, files, files_n);

    for (size_t i = 0; i < files_n; ++i) {
        FileWithMutex* cur = &files[i];
        FileWithMutexDestroy(cur);
    }
    free(files);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Wrong number of input arguments.\n"
                        "Program requires exactly two input arguments:\n"
                        "1. Path to the input directory;\n"
                        "2. Number of threads;\n");
        return 1;
    }

    const char* input_dir_path = argv[1];
    printf("Input directory path: %s\n", input_dir_path);

    const int n_threads = (int)strtol(argv[2], NULL, 0);
    printf("Number of threads: %d\n", n_threads);
    if (n_threads < 1) {
        fprintf(stderr, "Wrong number of input threads. It must be integer value, not less than 1.\n");
        return 1;
    }

    clock_t start = clock();

    Process(input_dir_path, (size_t)n_threads);

    clock_t end = clock();
    clock_t dur = end - start;
    double time_taken = ((double)dur) / CLOCKS_PER_SEC;
    printf("The program took %.5lf seconds to execute\n", time_taken);
    return 0;
}