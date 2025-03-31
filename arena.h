#ifndef ARENA_H
#define ARENA_H

typedef struct {
    _Generic* data,
    size_t size,
    size_t cap
} Arena;

Arena ArenaNew(size_t);
void* ArenaAllocate(Arena*, size_t);
void ArenaFree(Arena*);

#ifdef ARENA_IMPLEMENTATION

Arena ArenaNew(size_t initial_cap) {
    void* data = malloc(initial_cap);
    return (Arena){
        .data = data,
        .size = 0,
        .cap = initial_cap
    }
}

void* ArenaAllocate(Arena* arena, size_t size) {
    if (size + arena->size < arena->cap) {

    }

    uint8_t* data = &arena->data[arena->size];
    arena->size += size;
    return data;
}

void ArenaFree(Arena* a) {
    free(a->data);
    a.size = 0;
    a.cap = 0;
}

#endif // ARENA_IMPLEMENTATION
#endif // ARENA_H
