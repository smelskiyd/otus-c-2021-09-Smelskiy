//
// Created by Danya Smelskiy on 27.01.2022.
//

#ifndef OTUSHW_FILELIST_H
#define OTUSHW_FILELIST_H

#include <stdlib.h>

typedef struct FileNode FileNode;

struct FileNode {
    char* file_path;
    FileNode* next;
};

FileNode* InsertNode(FileNode* list_head, char* file_path);

void FreeList(FileNode* list_head);

size_t CountListSize(FileNode* list_head);

void PrintListOfFiles(FileNode* list_head);

#endif //OTUSHW_FILELIST_H
