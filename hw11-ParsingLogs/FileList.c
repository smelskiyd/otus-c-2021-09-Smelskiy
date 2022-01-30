//
// Created by Danya Smelskiy on 27.01.2022.
//

#include "FileList.h"

#include <stdio.h>

FileNode* InsertNode(FileNode* list_head, char* file_path) {
    FileNode* new_node = (FileNode*)(malloc(sizeof(FileNode)));
    new_node->next = list_head;
    new_node->file_path = file_path;
    return new_node;
}

void DestroyList(FileNode* list_head) {
    while (list_head != NULL) {
        FileNode* next = list_head->next;
        free(list_head->file_path);
        free(list_head);
        list_head = next;
    }
}

size_t CountListSize(FileNode* list_head) {
    size_t res = 0;
    while (list_head != NULL) {
        ++res;
        list_head = list_head->next;
    }
    return res;
}

void PrintListOfFiles(FileNode* list_head) {
    while (list_head != NULL) {
        printf("- %s\n", list_head->file_path);
        list_head = list_head->next;
    }
}
