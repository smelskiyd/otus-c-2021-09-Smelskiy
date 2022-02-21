//
// Created by Danya Smelskiy on 17.02.2022.
//

#ifndef OTUSHW_WORKER_H
#define OTUSHW_WORKER_H

#include <sys/types.h>

typedef struct {
    int fd;
    const char* directory_path;
    int* finish_flag;
} WorkerArgs;

void InitWorkers(size_t n);
void DestroyWorkers();
void RunWorker(int fd, const char* directory_path);

#endif //OTUSHW_WORKER_H
