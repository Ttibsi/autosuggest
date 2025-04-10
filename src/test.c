#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "dll.h"
#include "trie.h"

#define ARENA_IMPLEMENTATION
#include "arena.h"

#define call(func)                                      \
    {                                                   \
        func();                                         \
        printf("\x1b[32m%s succeeded\x1b[0m\n", #func); \
    };

void test_trie_insert() {
    Arena test_arena = { 0 };
    Trie* root = trieConstruct(&test_arena);

    trieInsert(root, 'A', false, &test_arena);
    assert(root->children_len == 1);
    assert(strcmp(root->children[0]->word, "A") == 0);

    trieInsert(root->children[0], 'B', true, &test_arena);
    assert(root->children_len == 1);
    assert(root->children[0]->children_len == 1);
    assert(strcmp(root->children[0]->word, "A") == 0);

    assert(root->children[0]->children[0]->children_len == 0);
    assert(strcmp(root->children[0]->children[0]->word, "AB") == 0);

    trieDestruct(&test_arena);
}

void test_trie_search() {
    Arena test_arena = { 0 };
    Trie* root = trieConstruct(&test_arena);

    trieInsert(root, 'A', false, &test_arena);
    trieInsert(root->children[0], 'B', true, &test_arena);

    Trie* found = trieSearch(root, "AB");
    assert(!(strcmp(found->word, "AB")));
    assert(found->terminal);

    trieDestruct(&test_arena);
}

void test_nodeLen() {
    Arena test_arena = { 0 };

    Node n1 = nodeCreate("foo", &test_arena);
    Node n2 = nodeCreate("bar", &test_arena);
    Node n3 = nodeCreate("baz", &test_arena);

    n1.next = &n2;
    n2.next = &n3;
    n2.prev = &n1;
    n3.prev = &n2;

    assert(nodeLen(&n1) == 3);

    arena_free(&test_arena);
}

void test_prefix_len() {
    assert(prefix_len("hell", "help") == 3);
    assert(prefix_len("no", "yes") == 0);
}

int main() {
    call(test_trie_insert);
    call(test_trie_search);
    call(test_nodeLen);
    call(test_prefix_len);
}
