#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define TRIE_IMPLEMENTATION
#include "trie.h"

#define call(func) { func(); printf("\x1b[32m%s succeeded\x1b[0m\n", #func); };

void test_trie_insert() {
    Trie* root = (Trie*)malloc(sizeof(Trie));
    *root = (Trie){
        .children_len = 0,
        .word = "\0",
        .terminal = false,
    };
    
    trieInsert(root, 'A', false);
    assert(root->children_len == 1);
    assert(strcmp(root->children[0]->word,"A") == 0);
    
}

int main() {
    call(test_trie_insert);
}
