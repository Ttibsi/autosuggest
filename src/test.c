#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "trie.h"

#define ARENA_IMPLEMENTATION
#include "arena.h"

#define call(func) { func(); printf("\x1b[32m%s succeeded\x1b[0m\n", #func); };

void test_trie_insert() {
    Arena test_arena = {0};
    Trie* root = trieConstruct(&test_arena);
    
    trieInsert(root, 'A', false);
    assert(root->children_len == 1);
    assert(strcmp(root->children[0]->word,"A") == 0);

    trieInsert(root->children[0], 'B', true);
    assert(root->children_len == 1);
    assert(root->children[0]->children_len == 1);
    assert(strcmp(root->children[0]->word,"A") == 0);

    assert(root->children[0]->children[0]->children_len == 0);
    assert(strcmp(root->children[0]->children[0]->word,"AB") == 0);

    trieDestruct(&test_arena);
}

void test_trie_search() {
    Arena test_arena = {0};
    Trie* root = trieConstruct(&test_arena);

    trieInsert(root, 'A', false);
    trieInsert(root->children[0], 'B', true);

    Trie* found = trieSearch(root, "AB");
    assert(!(strcmp(found->word, "AB")));
    assert(found->terminal);

    trieDestruct(&test_arena);
}

int main() {
    call(test_trie_insert);
    call(test_trie_search);
}
