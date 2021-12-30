//
// Created by Danya Smelskiy on 30.12.2021.
//

#ifndef OTUSHW_JSONVISUALIZER_H
#define OTUSHW_JSONVISUALIZER_H

#include "jansson.h"

void RecursivelyPrintJson(json_t* root, int indent);

void PrintIndent(int indent);
void PrintJsonInteger(json_t* root);
void PrintJsonReal(json_t* root);
void PrintJsonString(json_t* root);
void PrintJsonArray(json_t* root, int indent);
void PrintJsonObject(json_t* root, int indent);
void PrintJsonTrue();
void PrintJsonFalse();
void PrintJsonNull();

#endif //OTUSHW_JSONVISUALIZER_H
