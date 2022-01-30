//
// Created by Danya Smelskiy on 27.01.2022.
//

#ifndef OTUSHW_COMBINEDLOGFORMAT_H
#define OTUSHW_COMBINEDLOGFORMAT_H

#include <stdio.h>

// CommonLogFormat    "%h %l %u %t \"%r\" %>s %b"
// CombinedLogFormat  "%h %l %u %t \"%r\" %>s %b \"%{Referer}i\" \"%{User-agent}i\""

typedef struct {
    char* ip;               // %h
    char* l;                // %l
    char* user;             // %u
    char* time;             // %t
    char* request_line;     // "%r"
    char* status;           // %>s
    char* return_size;      // %b
    char* referer;          // "{Referer}i"
    char* user_agent;       // "{User-agent}i"
} CombinedLog;

void InitCombinedLog(CombinedLog* log);
// Rename as DestructCombinedLog
void FreeCombinedLog(CombinedLog* log);

CombinedLog* ParseCombinedLog(char* str);

long long StringToInteger(const char* return_size);

void PrintCombinedLog(const CombinedLog* log);

#endif //OTUSHW_COMBINEDLOGFORMAT_H
