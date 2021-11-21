//
// Created by Danya Smelskiy on 12.11.2021.
//

#ifndef OTUSHW_HASHTABLE_H
#define OTUSHW_HASHTABLE_H

#include <stdlib.h>

static const int kDefaultHashMapSize = 100;

/*
 * Stores the instance of the word with counter of the occurrences:
 * @param word The word
 * @param cnt The counter of the word occurrences
 */
typedef struct {
    char* word;
    int32_t cnt;
} Bucket;

/*
 * HashMap data structure with Open Addressing
 * @param size The size of the HashMap (the same as the capacity)
 * @param used_nodes The number of ever used nodes
 * @param table Buckets array
 * @param step The parameter used to iterate through the buckets in Find
 */

typedef struct {
    size_t size;
    size_t used_nodes;
    Bucket* table;
    size_t step;
} HashMap;

/*
 * The node of the list of Buckets
 * @param next The next node in the list
 * @param bucket The bucket
 */

struct BucketsListNode {
    struct BucketsListNode* next;
    Bucket* bucket;
};

typedef struct BucketsListNode BucketsListNode;

/*
 * Bucket destructor
 * @paragm cell The cell which will be destructed
 */

void DestructBucket(Bucket* cell);

/*
 * HashMap constructor
 * @param size The initial size of the HashMap
 */

HashMap* CreateHashMap(const size_t size);

/*
 * Expand the HashMap. Doubles the size of the HashMap
 * @param hash_map The input HashMap to expand
 */
void ExpandHashMap(HashMap* hash_map);

/*
 * HashMap destructor
 * @paragm hash_map The input HashMap
 */
void DestructHashMap(HashMap* hash_map);

/*
 * Find the specific word in HashMap
 * @param hash_map The input HashMap
 * @param word The word which we are searching for
 * @return Returns the first empty bucket in the search order,
 *         or the first and only instance of the word in HashMap
 *         Returns NULL if there are no instances of the word and
 *         the HashMap is full
 */
Bucket* Find(HashMap* hash_map, char* word);

/*
 * Insert the new word with the specified number of occurrences,
 *     or increase the counter if the word is already in HashMap
 * @param hash_map The input HashMap
 * @param word The word which want to insert
 * @param cnt The number of word occurrences
 */
void Insert(HashMap* hash_map, char* word, int32_t cnt);

/*
 * Remove the single occurrence of the word from the HashMap
 * @param hash_map The input HashMap
 * @param word The input word
 */
void Remove(HashMap* hash_map, char* word);

/*
 * Get the list of the all words with their occurrences in the HashMap
 * @param hash_map The input HashMap
 * @return Returns the list of the Buckets for each word with non-zero occurrences
 */
BucketsListNode* GetAllWords(HashMap* hash_map);

#endif //OTUSHW_HASHTABLE_H
