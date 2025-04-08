#ifndef DLL_H
#define DLL_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "arena.h"

typedef struct _node {
    struct _node* next;
    struct _node* prev;
    char* word;
    int word_len;
    bool selected;
} Node;

Node nodeCreate(const char*, Arena*);
size_t nodeLen(Node*);
void nodesFree(Node, Arena*);

#endif // DLL_H
