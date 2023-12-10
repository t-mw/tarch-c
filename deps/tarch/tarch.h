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

#define LOG(tag, ...)                                                              \
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
            fprintf(stderr, "LOG message too long for buffer\n");                  \
            break;                                                                 \
        }                                                                          \
                                                                                   \
        if (printf("%s [%s]: %s\n", time_str, tag, formatted_msg) < 0) {           \
            perror("printf");                                                      \
            break;                                                                 \
        }                                                                          \
    } while (0)

#if DEBUG
#define DEBUG_LOG(tag, ...) LOG("debug:" tag, __VA_ARGS__)
#else
#define DEBUG_LOG(tag, ...)
#endif

void* tm_malloc(size_t size);
void* tm_calloc(size_t n, size_t s);
void* tm_realloc(void* p, size_t s);
void tm_free(void* ptr);

#define tm_unused(_v) (void)(_v)
