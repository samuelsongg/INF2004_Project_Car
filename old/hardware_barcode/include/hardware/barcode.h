#ifndef _BARCODE_H
#define _BARCODE_H

#include <stddef.h>
#define TABLE_SIZE 100

// Define the structure for key-value pairs.
struct KeyValuePair {
    char* key;
    char* value;
    struct KeyValuePair* next;
};

extern struct KeyValuePair* myHashMap[TABLE_SIZE];

unsigned int hashFunction(const char* key);
void insertKeyValuePair(const char* key, const char* value);
void init_hashmap(void *params);
const char* getValue(const char *key);

#endif