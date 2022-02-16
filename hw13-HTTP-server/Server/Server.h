//
// Created by Danya Smelskiy on 17.02.2022.
//

#ifndef OTUSHW_SERVER_H
#define OTUSHW_SERVER_H

#include <stdint.h>

typedef struct {
    const char* directory_path;
} ServerArgs;

void RunServer(const char* address, uint32_t port, int backlog, ServerArgs* args);

#endif //OTUSHW_SERVER_H
