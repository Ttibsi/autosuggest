#ifndef DLL_H
#define DLL_H

#include <stddef.h>
#include <string.h>


typedef struct _Node {
    struct _Node* next;
    struct _Node* prev;
    char* word;
    int word_len;
    bool selected;
} Node;

Node nodeCreate(const char*);

# ifdef DLL_IMPLEMENTATION

static Arena dll_arena = {0};

Node nodeCreate(const char* word) {
    Node n;
    n.next = NULL;
    n.prev = NULL;
    n.selected = false;

    if (word != NULL) {
        n.word_len = strlen(word);
        n.word = (char*)arena_alloc(&dll_arena, sizeof(char) * (strlen(word) + 1));
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

void nodesFree(Node n) {
    while (n.next != NULL) {
        free(n.word);
        n = *n.next;
        // free(n.prev);
    }
}

#endif // DLL_IMPLEMENTATION
#endif // DLL_H
