#ifndef H
#define H

#define var __auto_type
#define let __auto_type const
#define defer(x) __attribute__((cleanup(x)))

#ifndef NDEBUG
#  define DBG 1
#else
#  define DBG 0
#endif

#define PRINT_ARR(format, arr)                                  \
  for (size_t i = 0; i < (sizeof(arr) / sizeof(arr[0])); i++) { \
    printf("%zu: " format "\n", i, arr[i]);                     \
  }

#include <stdio.h>
#include <time.h>
#define LOG(tag, ...)                                              \
  do {                                                             \
    static char formatted_msg[256];                                \
    static char time_str[32];                                      \
    time_t now;                                                    \
    struct tm* timeinfo;                                           \
                                                                   \
    time(&now);                                                    \
    timeinfo = localtime(&now);                                    \
                                                                   \
    strftime(time_str, sizeof(time_str), "%F %H:%M:%S", timeinfo); \
    snprintf(formatted_msg, sizeof(formatted_msg), __VA_ARGS__);   \
                                                                   \
    printf("%s [%s]: %s\n", time_str, tag, formatted_msg);         \
  } while (0)
#if DBG
#  define DBG_LOG(tag, ...) LOG("debug:" tag, __VA_ARGS__)
#else
#  define DBG_LOG(tag, ...)
#endif

void* tarch_malloc(size_t size, void* user_data);
void* tarch_calloc(size_t n, size_t s, void* user_data);
void* tarch_realloc(void* p, size_t s, void* user_data);
void tarch_free(void* ptr, void* user_data);

#endif
