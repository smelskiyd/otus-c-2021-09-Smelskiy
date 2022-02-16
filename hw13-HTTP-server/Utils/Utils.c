//
// Created by Danya Smelskiy on 19.02.2022.
//

#include "Utils.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void ParseAddressAndPort(const char* str, char** address, uint32_t* port) {
    int colon_pos = -1;
    const size_t str_len = strlen(str);
    for (size_t i = 0; i < str_len; ++i) {
        if (str[i] == ':') {
            colon_pos = (int)i;
            break;
        }
    }

    if (colon_pos == -1) {
        fprintf(stderr, "Failed to parse address '%s': colon symbol is missed.\n", str);
        exit(EXIT_FAILURE);
    }

    const size_t addr_len = colon_pos;
    *address = (char*)malloc(addr_len + 1);
    strncpy(*address, str, addr_len);
    (*address)[addr_len] = '\0';

    *port = strtoul(str + colon_pos + 1, NULL, 10);
}

char* ConcatenateStrings(const char* lhs, const char* rhs) {
    size_t len = strlen(lhs) + strlen(rhs) + 1;
    char* result = (char*)malloc(len);
    snprintf(result, len, "%s%s", lhs, rhs);
    return result;
}
