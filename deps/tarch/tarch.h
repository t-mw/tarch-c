#pragma once

#define var __auto_type
#define let __auto_type const
#define defer(x) __attribute__((cleanup(x)))

#ifndef NDEBUG
#define DEBUG 1
#else
#define DEBUG 0
#endif

#define PRINT_ARR(format, arr)                                    \
    for (size_t i = 0; i < (sizeof(arr) / sizeof(arr[0])); i++) { \
        printf("%zu: " format "\n", i, arr[i]);                   \
    }

#include <pthread.h>
#include <stdio.h>
#include <time.h>

#define LOG_MAX_MSG_LEN 256
#define LOG_MAX_TIME_LEN 32

#define tm_log(tag, ...)                                                           \
    do {                                                                           \
        char formatted_msg[LOG_MAX_MSG_LEN];                                       \
        char time_str[LOG_MAX_TIME_LEN];                                           \
        time_t now;                                                                \
        struct tm timeinfo;                                                        \
                                                                                   \
        time(&now);                                                                \
        localtime_r(&now, &timeinfo);                                              \
                                                                                   \
        if (strftime(time_str, sizeof(time_str), "%F %H:%M:%S", &timeinfo) == 0) { \
            fprintf(stderr, "strftime error\n");                                   \
            break;                                                                 \
        }                                                                          \
                                                                                   \
        int ret = snprintf(formatted_msg, sizeof(formatted_msg), __VA_ARGS__);     \
        if (ret < 0) {                                                             \
            perror("snprintf");                                                    \
            break;                                                                 \
        } else if (ret >= (int)sizeof(formatted_msg)) {                            \
            fprintf(stderr, "log message too long for buffer\n");                  \
            break;                                                                 \
        }                                                                          \
                                                                                   \
        if (printf("%s [%s]: %s\n", time_str, tag, formatted_msg) < 0) {           \
            perror("printf");                                                      \
            break;                                                                 \
        }                                                                          \
    } while (0)

#if DEBUG
#define tm_debug_log(tag, ...) tm_log("debug:" tag, __VA_ARGS__)
#else
#define tm_debug_log(tag, ...)
#endif

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
static inline void _tm_print_ptr(void* v) {
    printf("%p", v);
}

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
        default: _tm_print_ptr)(val)

#define tm_debug_print(val)                \
    do {                                   \
        if (DEBUG) {                       \
            printf("value of %s: ", #val); \
            _tm_generic_print(val);        \
            printf("\n");                  \
        }                                  \
    } while (0)

void* tm_malloc(size_t size);
void* tm_calloc(size_t n, size_t s);
void* tm_realloc(void* p, size_t s);
void tm_free(void* ptr);

#define tm_unused(_v) (void)(_v)
