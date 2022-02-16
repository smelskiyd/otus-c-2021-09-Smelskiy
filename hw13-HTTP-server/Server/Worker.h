//
// Created by Danya Smelskiy on 17.02.2022.
//

#ifndef OTUSHW_WORKER_H
#define OTUSHW_WORKER_H

typedef struct {
    int fd;
    const char* directory_path;
} WorkerArgs;

void RunWorker(WorkerArgs* args);

#endif //OTUSHW_WORKER_H
