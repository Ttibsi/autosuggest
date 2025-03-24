#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// https://en.wikipedia.org/wiki/Trie
#define ALPHABET_LEN 26

typedef struct trie {
    struct trie* children[ALPHABET_LEN];
    size_t children_len;
    bool terminal;
    char letter;
} Trie;

extern inline void trieInsert(Trie* t, char c);
extern inline Trie* trieSearch(Trie* t, char* str);
extern inline void trieDelete(Trie* t, char* str);

#ifdef TRIE_IMPLEMENTATION

inline void trieInsert(Trie* t, char c) {
    if (!(t->children_len)) { return; }

    // Check if the letter is already present in the trie's children
    for (size_t i = 0; i < t->children_len; i++) {
        if (t->children[i]->letter == c) {
            return;
        }
    }

    t->children[t->children_len] = (Trie*)malloc(sizeof(Trie));
    *t->children[t->children_len] = (Trie){
        .children = {NULL},
        .children_len = 0,
        .terminal = true,
        .letter = c
    };

    t->children_len++;
    t->terminal = false;
    return;
}

inline Trie* trieSearch(Trie* t, char* str) {
    if (!(t->children_len)) { return t; }

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

inline void trieDelete(Trie* t, char* str) {
    if (strlen(str)) {
        char c = str[strlen(str) - 1];
        str[strlen(str) - 1] = '\0';

        Trie* parent = trieSearch(t, str);
        for (size_t i = 0; i < t->children_len; i++) {
            if (parent->children[i]->letter == c) {
                // Should we move later children up forward? How?
                parent->children[i] = NULL;
                parent->children_len--;
                if (!(parent->children_len)) { parent->terminal = true; }
                return;
            }
        }
    }
}

#endif // TRIE_IMPLEMENTATION
#endif // TRIE_H
