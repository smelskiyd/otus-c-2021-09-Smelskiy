//
// Created by Danya Smelskiy on 07.11.2021.
//

#include <assert.h>

#include "iso8859-5.h"

// Last 128 codes of iso8859-5 in Unicode
int iso8859toUnicode[128];

void InitISO8859Encoding() {
    int last_pos = 0;
    for (int i = 0x80; i <= 0xA0; ++i) {
        iso8859toUnicode[last_pos++] = i;
    }
    for (int i = 0x401; i <= 0x44f; ++i) {
        iso8859toUnicode[last_pos++] = i;
    }
    iso8859toUnicode[last_pos++] = 0x2116;
    for (int i = 0x451; i <= 0x45c; ++i) {
        iso8859toUnicode[last_pos++] = i;
    }
    iso8859toUnicode[last_pos++] = 0xA7;
    iso8859toUnicode[last_pos++] = 0x45E;
    iso8859toUnicode[last_pos++] = 0x45F;

    assert(last_pos == 128);
}

int ConvertISO8859ToUnicode(int c) {
    if (c < 128) {
        return c;
    }
    return iso8859toUnicode[c - 128];
}