#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>
#include <stddef.h>

// https://en.wikipedia.org/wiki/Trie
#define ALPHABET_LEN 26

typedef struct trie {
    struct trie* children[ALPHABET_LEN];
    size_t children_len;
    bool terminal;
    char letter;
} Trie;

void trieInsert(Trie*, char);
Trie* trieSearch(Trie*, char*);
void trieDelete(Trie*, char*);

#endif // TRIE_H
