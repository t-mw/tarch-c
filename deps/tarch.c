#include <tarch/tarch.h>

#include <stdlib.h>

void* tarch_malloc(size_t size) {
    return malloc(size);
}

void* tarch_calloc(size_t n, size_t s) {
    return calloc(n, s);
}

void* tarch_realloc(void* p, size_t s) {
    return realloc(p, s);
}

void tarch_free(void* ptr) {
    free(ptr);
}
