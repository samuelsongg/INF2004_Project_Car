#include "hardware/barcode.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Define the hash map as an array of pointers to KeyValuePair.
struct KeyValuePair* myHashMap[TABLE_SIZE] = {NULL};

// Hash function to determine array index.
unsigned int hashFunction(const char* key) {
    unsigned int hash = 0;

    for (unsigned int i = 0; i < strlen(key); i++) {
        hash = hash * 31 + key[i];
    }

    return hash % TABLE_SIZE;
}

// Function to insert a key-value pair into the hash map.
void insertKeyValuePair(const char* key, const char* value) {
    unsigned int index = hashFunction(key);

    struct KeyValuePair* newPair = malloc(sizeof(struct KeyValuePair));
    newPair->key = strdup(key);
    newPair->value = strdup(value);
    newPair->next = NULL;

    if (myHashMap[index] == NULL) {
        myHashMap[index] = newPair;
    }
    else {
        struct KeyValuePair* current = myHashMap[index];
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newPair;
    }
}

// Function to initialise the hash map for barcode.
void init_hashmap(void *params) {

    // Insert key-value pairs for barcode scanner.
    insertKeyValuePair("1010001110111010", "0");
    insertKeyValuePair("1110100010101110", "1");
    insertKeyValuePair("1011100010101110", "2");
    insertKeyValuePair("1110111000101010", "3");
    insertKeyValuePair("1010001110101110", "4");
    insertKeyValuePair("1110100011101010", "5");
    insertKeyValuePair("1011100011101010", "6");
    insertKeyValuePair("1010001011101110", "7");
    insertKeyValuePair("1110100010111010", "8");
    insertKeyValuePair("1011100010111010", "9");
    insertKeyValuePair("1110101000101110", "A");
    insertKeyValuePair("1011101000101110", "B");
    insertKeyValuePair("1110111010001010", "C");
    insertKeyValuePair("1010111000101110", "D");
    insertKeyValuePair("1110101110001010", "E");
    insertKeyValuePair("1011101110001010", "F");
    insertKeyValuePair("1010100011101110", "G");
    insertKeyValuePair("1110101000111010", "H");
    insertKeyValuePair("1011101000111010", "I");
    insertKeyValuePair("1010111000111010", "J");
    insertKeyValuePair("1110101010001110", "K");
    insertKeyValuePair("1011101010001110", "L");
    insertKeyValuePair("1110111010100010", "M");
    insertKeyValuePair("1010111010001110", "N");
    insertKeyValuePair("1110101110100010", "O");
    insertKeyValuePair("1011101110100010", "P");
    insertKeyValuePair("1010101110001110", "Q");
    insertKeyValuePair("1110101011100010", "R");
    insertKeyValuePair("1011101011100010", "S");
    insertKeyValuePair("1010111011100010", "T");
    insertKeyValuePair("1110001010101110", "U");
    insertKeyValuePair("1000111010101110", "V");
    insertKeyValuePair("1110001110101010", "W");
    insertKeyValuePair("1000101110101110", "X");
    insertKeyValuePair("1110001011101010", "Y");
    insertKeyValuePair("1000111011101010", "Z");
    insertKeyValuePair("1000101110111010", "*");
    insertKeyValuePair("1000101011101110", "-");
    insertKeyValuePair("1110001010111010", ".");
    insertKeyValuePair("1000111010111010", " ");
    insertKeyValuePair("1000100010001010", "$");
    insertKeyValuePair("1000100010100010", "/");
    insertKeyValuePair("1000101000100010", "+");
    insertKeyValuePair("1010001000100010", "%");
}

// Function to retrieve the value associated with a key.
const char* getValue(const char* key) {
    unsigned int index = hashFunction(key);
    struct KeyValuePair* current = myHashMap[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return NULL;  // Key not found
}
/*** End of file ***/