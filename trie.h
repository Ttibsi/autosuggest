#ifndef TRIE_H
#define TRIE_H

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// https://en.wikipedia.org/wiki/Trie
#define ALPHABET_LEN 70

// `terminal` flag means it's a whole word, doesn't mean it can't have children
// ex the word `ten` would have a terminal flag, even though `tend` is a legal
// word stored as a child of `ten`
typedef struct trie {
    struct trie* children[ALPHABET_LEN];
    size_t children_len;
    char* word;
    bool terminal;
} Trie;

void trieInsert(Trie*, char, bool);
Trie* trieSearch(Trie*, char*);

#ifdef TRIE_IMPLEMENTATION

#if 0
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

#else

void trieInsert(Trie* t, char c, bool terminal) {
    if (t == NULL) { return; }
    if (isspace(c)) { return; }

    char* input_word = malloc(55 * sizeof(char));
    sprintf(input_word, "%s%c", t->word, c);

    if (t->children_len == ALPHABET_LEN) {
        fprintf(stderr, "Word is too long: %c(%i) not included\n", c, c);
    }

    for (size_t i = 0; i < t->children_len; i++) {
        if (t->children[i]->word == input_word) { return; }
    }

    Trie* new_node = (Trie*)malloc(sizeof(Trie));
    *new_node = (Trie){
        .children_len = 0,
        .word = input_word,
        .terminal = terminal
    };

    t->children[t->children_len++] = new_node;
}

Trie* trieSearch(Trie* t, char* word) {
    if (t == NULL) { return NULL; }
    if (strlen(word) == 0) { return NULL; }

    for (size_t i = 0; i < strlen(word); i++) {
        bool found = false;
        for (size_t j = 0; j < ALPHABET_LEN; j++) {
            if (t->children[j] == NULL) { break; }

            if (strspn(word, t->children[j]->word) == strlen(word)) {
                t = t->children[j];
                found = true;
                break;
            }
        }
        // Failed to find
        if (!(found)) { return NULL; }
    }

    return t;
}

#endif

#endif // TRIE_IMPLEMENTATION
#endif // TRIE_H
