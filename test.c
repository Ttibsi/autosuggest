#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define TRIE_IMPLEMENTATION
#include "trie.h"

#define call(func) { func(); printf("\x1b[32m%s succeeded\x1b[0m\n", #func); };

void test_trie_insert() {
    Trie* root = trieConstruct();
    
    trieInsert(root, 'A', false);
    assert(root->children_len == 1);
    assert(strcmp(root->children[0]->word,"A") == 0);

    trieInsert(root->children[0], 'B', true);
    assert(root->children_len == 1);
    assert(root->children[0]->children_len == 1);
    assert(strcmp(root->children[0]->word,"A") == 0);

    assert(root->children[0]->children[0]->children_len == 0);
    assert(strcmp(root->children[0]->children[0]->word,"AB") == 0);

    trieDestruct();
}

void test_trie_search() {
    Trie* root = trieConstruct();
    trieInsert(root, 'A', false);
    trieInsert(root->children[0], 'B', true);

    Trie* found = trieSearch(root, "AB");
    assert(!(strcmp(found->word, "AB")));
    assert(found->terminal);

    trieDestruct();
}

int main() {
    call(test_trie_insert);
    call(test_trie_search);
}
