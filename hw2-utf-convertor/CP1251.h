//
// Created by Danya Smelskiy on 06.11.2021.
//

#ifndef OTUSHW_CP1251_H
#define OTUSHW_CP1251_H

// Last 128 codes of CP1251 in Unicode
extern int CP1251toUnicode[128];

void InitCP1251Encoding();

int ConvertCP1251ToUnicode(int c);

#endif //OTUSHW_CP1251_H
