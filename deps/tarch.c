#include <tarch/tarch.h>

#include <stdlib.h>

void* tm_malloc(size_t size) {
    return malloc(size);
}

void* tm_calloc(size_t n, size_t s) {
    return calloc(n, s);
}

void* tm_realloc(void* p, size_t s) {
    return realloc(p, s);
}

void tm_free(void* ptr) {
    free(ptr);
}
