#include <string.h>

#include "trie.h"

void trieInsert(Trie* t, char c) {
    // Check if the letter is already present in the trie's children
    for (size_t i = 0; i < t->children_len; i++) {
        if (t->children[t->children_len]->letter == c) {
            return;
        }
    }

    t->children[t->children_len] = &((Trie){
        .children_len = 0,
        .terminal = false,
        .letter = c
    });

    t->children_len++;
}

Trie* trieSearch(Trie* t, char* str) {
    Trie* new_t = NULL;

    for (size_t i = 0; i < strlen(str); i++) {
        for (size_t j = 0; j < t->children_len; j++) {
            if (t->children[j]->letter == str[i]) {
                new_t = t->children[j];
                break;
            }

            new_t = NULL;
        }

        if (new_t == NULL) {
            return NULL;
        }

        t = new_t;
        if (i == strlen(str) - 1) { t->terminal = true; }

    }
    return new_t;
}

void trieDelete(Trie* t, char* str) {

}
