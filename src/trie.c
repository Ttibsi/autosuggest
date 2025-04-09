#include "trie.h"

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

Trie* trieConstruct(Arena* arena) {
    Trie* memory = arena_alloc(arena, 1024 * 5);
    *memory = (Trie){
        .children_len = 0,
        .word = "\0",
        .terminal = false,
    };

    return memory;
}

void trieDestruct(Arena* arena) {
    arena_free(arena);
}

void trieInsert(Trie* t, char c, bool terminal, Arena* arena) {
    if (t == NULL) { return; }
    if (isspace(c)) { return; }

    char* input_word = arena_alloc(arena, 55 * sizeof(char));
    sprintf(input_word, "%s%c", t->word, c);

    for (size_t i = 0; i < t->children_len; i++) {
        if (strcmp(t->children[i]->word, input_word) == 0) { return; }
    }

    Trie* new_node = arena_alloc(arena, sizeof(Trie));
    *new_node = (Trie){
        .children_len = 0,
        .word = input_word,
        .terminal = terminal
    };

    t->children[t->children_len++] = new_node;
}

size_t prefix_len(char* lhs, char* rhs) {
    size_t counter = 0;
    while(lhs[counter] == rhs[counter]) { 
        if (lhs[counter] == '\0') { break; }
        counter++;
    }
    return counter;
}

Trie* trieSearch(Trie* t, char* word) {
    if (t == NULL) { return NULL; }
    if (strlen(word) == 0) { return t; }

    for (size_t i = 0; i < strlen(word); i++) {
        bool found = false;

        for (size_t j = 0; j < t->children_len; j++) {
            if (t->children[j] == NULL) { break; }

            if (prefix_len(word, t->children[j]->word) == i + 1) {
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
