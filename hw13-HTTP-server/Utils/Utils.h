//
// Created by Danya Smelskiy on 19.02.2022.
//

#ifndef OTUSHW_UTILS_H
#define OTUSHW_UTILS_H

#include <stdint.h>

void ParseAddressAndPort(const char* str, char** address, uint32_t* port);

char* ConcatenateStrings(const char* lhs, const char* rhs);

#endif //OTUSHW_UTILS_H
