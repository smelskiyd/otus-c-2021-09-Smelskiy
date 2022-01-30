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
    atomic_bool done;  // Fully read
} FileWithMutex;

void FileWithMutexInit(FileWithMutex* file_with_mutex, char* file_path);
void FileWithMutexDestroy(FileWithMutex* file_with_mutex);

/*
 * Tries to find and lock any not locked and not already finished
 * file while there is at least one not finished file.
 * @param files Array of files with mutexes
 * @param files_n Length of the array 'files'
 * @return Returns identifier of the locked file.
 *         If all files are finished returns -1.
 */
int FindReadyFileAndLock(FileWithMutex* files, size_t files_n);

#endif //OTUSHW_FILEWITHMUTEX_H
