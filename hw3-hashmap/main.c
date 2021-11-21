//
// Created by Danya Smelskiy on 12.11.2021.
//

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "HashMap.h"

static const int kMaxWordLength = 255;

char* ReadNextWord(FILE* input_file) {
    static char buffer[kMaxWordLength + 1];

    if (fscanf(input_file, "%s", buffer) != 1) {
        return NULL;
    }

    size_t len = strlen(buffer);
    char* result = (char*)malloc(len + 1);
    return strcpy(result, buffer);
}

void PrintHashMap(HashMap* hash_map) {
    BucketsListNode* words_list;
    words_list = GetAllWords(hash_map);
    while (words_list != NULL) {
        printf("The word '%s' has %d occurrences in the file\n",
               words_list->bucket->word, words_list->bucket->cnt);
        BucketsListNode* next = words_list->next;
        free(words_list);
        words_list = next;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Incorrect number of input arguments.\n"
               "Program requires exactly one argument: the path to the input file.\n");
        return 1;
    }

    const char* const input_path = argv[1];
    printf("Path to the input file: %s\n", input_path);

    FILE* input_file;
    input_file = fopen(input_path, "r");
    if (!input_file) {
        perror("Can't open the input file");
        return errno;
    }

    HashMap* hash_map;
    hash_map = CreateHashMap(kDefaultHashMapSize);

    while (1) {
        char* next_word = ReadNextWord(input_file);
        if (next_word == NULL) {
            break;
        }
        Insert(hash_map, next_word, 1);
    }

    PrintHashMap(hash_map);

    DestructHashMap(hash_map);

    fclose(input_file);

    return 0;
}
