//
// Created by Danya Smelskiy on 07.11.2021.
//

#ifndef OTUSHW_ISO8859_5_H
#define OTUSHW_ISO8859_5_H

#include <assert.h>

// Last 128 codes of iso8859-5 in Unicode
extern int iso8859toUnicode[128];

void InitISO8859Encoding();

int ConvertISO8859ToUnicode(int c);

#endif //OTUSHW_ISO8859_5_H
