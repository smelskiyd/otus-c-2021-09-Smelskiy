//
// Created by Danya Smelskiy on 12.11.2021.
//

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "HashMap.h"

#include "Hash.h"

bool IsPrime(size_t n) {
    for (size_t i = 2; i * i <= n; ++i) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

size_t GenerateStep() {
    size_t step = (size_t)rand();
    while (!IsPrime(step)) {
        ++step;
    }
    return step;
}

void DestructBucket(Bucket* bucket) {
    free(bucket->word);
}

void DestructTable(Bucket* table, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        Bucket* bucket = &table[i];
        if (bucket->cnt != -1) {
            DestructBucket(bucket);
        }
    }
    free(table);
}

HashMap* CreateHashMap(const size_t size) {
    HashMap* hash_map = (HashMap*)malloc(sizeof(HashMap));
    hash_map->size = size;
    hash_map->used_nodes = 0u;
    hash_map->table = (Bucket*)malloc(sizeof(Bucket) * size);
    hash_map->step = GenerateStep();
    for (size_t i = 0; i < size; ++i) {
        Bucket* cur_node = &hash_map->table[i];
        // Value -1 represents non-used elements
        cur_node->cnt = -1;
    }
    return hash_map;
}

HashMap* MergeHashMaps(HashMap* fir, HashMap* sec) {
    BucketsListNode* words_list;
    words_list = GetAllWords(sec);

    while (words_list != NULL) {
        char* word = words_list->bucket->word;
        char* word_copy = NULL;
        word_copy = (char*)(malloc(strlen(word) + 1));
        strncpy(word_copy, word, strlen(word) + 1);

        Insert(fir, word_copy, words_list->bucket->cnt);

        BucketsListNode* next = words_list->next;
        free(words_list);
        words_list = next;
    }

    return fir;
}

void ExpandHashMap(HashMap* hash_map) {
    Bucket* old_table = hash_map->table;
    const size_t old_size = hash_map->size;

    hash_map->size *= 2;
    hash_map->used_nodes = 0u;

    hash_map->table = (Bucket*)malloc(sizeof(Bucket) * hash_map->size);
    for (size_t i = 0; i < hash_map->size; ++i) {
        Bucket* cur_node = &hash_map->table[i];
        // Value -1 represents non-used element
        cur_node->cnt = -1;
    }

    for (size_t i = 0; i < old_size; ++i) {
        Bucket* cur_node = &old_table[i];
        if (cur_node->cnt > 0) {
            Insert(hash_map, cur_node->word, cur_node->cnt);
        } else if (cur_node->cnt == 0) {
            // This word was added at least once, but was removed the same number of times
            // So now there are no instances of this word in the HashMap
            DestructBucket(cur_node);
        }
    }

    free(old_table);
}

void DestructHashMap(HashMap* hash_map) {
    DestructTable(hash_map->table, hash_map->size);
    free(hash_map);
}

Bucket* Find(HashMap* hash_map, char* word) {
    const size_t start = GetHash(word) % hash_map->size;
    size_t pos = start;
    size_t step = hash_map->step;
    while (true) {
        if (hash_map->table[pos].cnt == -1 || strcmp(word, hash_map->table[pos].word) == 0) {
            // Return the first empty bucket or the first and only instance of the word
            return &hash_map->table[pos];
        }

        pos = (pos + step) % hash_map->size;
        if (pos == start) {
            break;
        }
    }
    return NULL;
}

void Insert(HashMap* hash_map, char* word, int32_t cnt) {
    Bucket* bucket = Find(hash_map, word);
    if (bucket == NULL) {
        printf("Can't insert new word in HashTable\n");
        exit(1);
    }
    if (bucket->cnt == -1) {
        // Create new
        bucket->cnt = cnt;
        bucket->word = word;
        hash_map->used_nodes++;
    } else {
        // Increase the counter for the found instance
        bucket->cnt += cnt;
        // We need to free the memory allocated for 'word',
        // because it's already allocated in HashMap
        free(word);
    }

    // Expand HashMap size
    if (hash_map->used_nodes > (hash_map->size / 2)) {
        ExpandHashMap(hash_map);
    }
}

void Remove(HashMap* hash_map, char* word) {
    Bucket* bucket = Find(hash_map, word);
    if (bucket == NULL || bucket->cnt <= 0) {
        printf("Instance of the word '%s' is not found\n", word);
        return;
    }
    bucket->cnt--;
}

BucketsListNode* GetAllWords(HashMap* hash_map) {
    BucketsListNode* head = NULL;
    BucketsListNode* tail = NULL;
    for (size_t i = 0; i < hash_map->size; ++i) {
        Bucket* bucket = &hash_map->table[i];
        if (bucket->cnt > 0) {
            BucketsListNode* new_node = (BucketsListNode*)(malloc(sizeof(BucketsListNode)));
            new_node->bucket = bucket;
            new_node->next = NULL;
            if (head == NULL) {
                head = new_node;
                tail = new_node;
            } else {
                tail->next = new_node;
                tail = new_node;
            }
        }
    }

    return head;
}

