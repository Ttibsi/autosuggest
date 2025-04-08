#include "dll.h"
#include <stdlib.h>

Node nodeCreate(const char* word, Arena* dll_arena) {
    Node n;
    n.next = NULL;
    n.prev = NULL;
    n.selected = false;

    if (word != NULL) {
        n.word_len = strlen(word);
        n.word = arena_alloc(dll_arena, sizeof(char) * (strlen(word) + 1));
        n.word = strcpy(n.word, word);
    } else {
        n.word_len = 0;
        n.word = "";
    }
    return n;
}

size_t nodeLen(Node* n) {
    size_t i = 1;
    while (n->next != NULL) {
        i++;
        n = n->next;
    }

    return i;
}

void nodesFree(Node n, Arena* dll_arena) {
    while (n.next != NULL) {
        n = *n.next;
        free(n.prev);
    }

    arena_free(dll_arena);
}
