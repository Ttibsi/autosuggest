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

Trie* trieConstruct(void);
void trieDestruct(void);
void trieInsert(Trie*, char, bool);
Trie* trieSearch(Trie*, char*);

#ifdef TRIE_IMPLEMENTATION

#define ARENA_IMPLEMENTATION
#include "arena.h"

static Arena default_arena = {0};
Trie* trieConstruct(void) {
    Trie* memory = (Trie*)arena_alloc(&default_arena, 1024 * 5);
    *memory = (Trie){
        .children_len = 0,
        .word = "\0",
        .terminal = false,
    };

    return memory;
}

void trieDestruct(void) {
    arena_free(&default_arena);
}

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

#endif // TRIE_IMPLEMENTATION
#endif // TRIE_H
