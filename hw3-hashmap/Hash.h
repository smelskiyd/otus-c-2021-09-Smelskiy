//
// Created by Danya Smelskiy on 20.11.2021.
//

#pragma once

#include <stdio.h>

static const size_t kHashKey = 113;
static const size_t kHashMod = (size_t)1e9 + 7;

size_t GetCharCode(char c);

size_t GetHash(const char* const word);
