#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// https://en.wikipedia.org/wiki/Trie
#define ALPHABET_LEN 70

typedef struct trie {
    struct trie* children[ALPHABET_LEN];
    size_t children_len;
    bool terminal;
    char letter;
} Trie;

void trieInsert(Trie*, char);
Trie* trieSearch(Trie*, char*);
void trieDelete(Trie*, char*);
void trieDestroy(Trie*);

#ifdef TRIE_IMPLEMENTATION

void trieInsert(Trie* t, char c) {
    if (t == NULL) { return; }

    // Check if the letter is already present in the trie's children
    for (size_t i = 0; i < t->children_len; i++) {
        if (t->children[i] && t->children[i]->letter == c) {
            return; // Letter already exists
        }
    }

    // Find first available slot
    if (t->children_len >= ALPHABET_LEN) {
        fprintf(stderr, "Warning: Maximum children reached\n");
        return;
    }

    // Create new node
    t->children[t->children_len] = (Trie*)malloc(sizeof(Trie));
    *t->children[t->children_len] = (Trie){
        .children = {NULL},
        .children_len = 0,
        .terminal = true, // Only set terminal at word endings
        .letter = c
    };

    t->children_len++;
    return;
}

Trie* trieSearch(Trie* t, char* str) {
    // Early return if input is NULL or empty
    if (!t || !str ) { return NULL; }

    Trie* new_t = NULL;

    for (size_t i = 0; i < strlen(str); i++) {
        new_t = NULL;
        bool found = false;

        if (t->terminal || t->children_len == 0) { return t; }

        for (size_t j = 0; j < t->children_len; j++) {
            if (t->children[j] && t->children[j]->letter == str[i]) {
                new_t = t->children[j];
                found = true;
                break;
            }
        }

        t = new_t;
    }

    return t;
}

void trieDelete(Trie* t, char* str) {
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

void trieDestroy(Trie* t) {
    if (t == NULL) return;

    // Recursively free all children
    for (size_t i = 0; i < t->children_len; i++) {
        if (t->children[i] != NULL) {
            // Recursively destroy each child
            trieDestroy(t->children[i]);
        }
    }

    // Free the current node
    free(t);
}

#endif // TRIE_IMPLEMENTATION
#endif // TRIE_H
