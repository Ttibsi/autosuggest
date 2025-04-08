#ifndef TRIE_H
#define TRIE_H

#include "arena.h"

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

Trie* trieConstruct(Arena*);
void trieDestruct(Arena*);
void trieInsert(Trie*, char, bool);
Trie* trieSearch(Trie*, char*);

#endif // TRIE_H
