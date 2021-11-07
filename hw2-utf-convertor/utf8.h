//
// Created by Danya Smelskiy on 07.11.2021.
//

#ifndef OTUSHW_UTF8_H
#define OTUSHW_UTF8_H

extern const int kUTF8BOM;

int GetNumberOfOctets(int c);

int ConvertUnicodeToUTF8(int uni_c);

#endif //OTUSHW_UTF8_H
