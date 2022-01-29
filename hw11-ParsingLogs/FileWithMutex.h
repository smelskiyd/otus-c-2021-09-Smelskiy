//
// Created by Danya Smelskiy on 29.01.2022.
//

#ifndef OTUSHW_FILEWITHMUTEX_H
#define OTUSHW_FILEWITHMUTEX_H

#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

typedef struct {
    FILE* file;
    // TODO: use static mutexes
    pthread_mutex_t* mutex;
    atomic_bool done;
} FileWithMutex;

void FileWithMutexInit(FileWithMutex* file_with_mutex, char* file_path);
void FileWithMutexDestroy(FileWithMutex* file_with_mutex);

int FindReadyFileAndLock(FileWithMutex* files, size_t files_n);

#endif //OTUSHW_FILEWITHMUTEX_H
