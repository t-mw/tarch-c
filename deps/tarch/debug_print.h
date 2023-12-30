#pragma once

#include <stdio.h>

static inline void _tm_print_char(char v) {
    printf("%c", v);
}
static inline void _tm_print_int(int v) {
    printf("%d", v);
}
static inline void _tm_print_uint(unsigned int v) {
    printf("%u", v);
}
static inline void _tm_print_ushort(unsigned short v) {
    printf("%hu", v);
}
static inline void _tm_print_float(float v) {
    printf("%f", v);
}
static inline void _tm_print_double(double v) {
    printf("%f", v);
}
static inline void _tm_print_long(long v) {
    printf("%ld", v);
}
static inline void _tm_print_ulong(unsigned long v) {
    printf("%lu", v);
}
static inline void _tm_print_llong(long long v) {
    printf("%lld", v);
}
static inline void _tm_print_ullong(unsigned long long v) {
    printf("%llu", v);
}
static inline void _tm_print_noop(void* v) {
    tm_unused(v);
}

// application can define tm_debug_print_custom as _Generic statement to print custom types
#ifndef tm_debug_print_custom
#define tm_debug_print_custom(val) _tm_print_noop
#endif

#define _tm_generic_print(val)                \
    _Generic((val),                           \
        char: _tm_print_char,                 \
        int: _tm_print_int,                   \
        unsigned int: _tm_print_uint,         \
        unsigned short: _tm_print_ushort,     \
        float: _tm_print_float,               \
        double: _tm_print_double,             \
        long: _tm_print_long,                 \
        unsigned long: _tm_print_ulong,       \
        long long: _tm_print_llong,           \
        unsigned long long: _tm_print_ullong, \
        default: tm_debug_print_custom(val))(val)

#define tm_debug_print(val)                \
    do {                                   \
        if (DEBUG) {                       \
            printf("value of %s: ", #val); \
            _tm_generic_print(val);        \
            printf("\n");                  \
        }                                  \
    } while (0)
