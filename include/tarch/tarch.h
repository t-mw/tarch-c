#ifndef TARCH_MAX_PATH
#  define TARCH_MAX_PATH 256
#endif

#define var __auto_type
#define let __auto_type const
#define defer(x) __attribute__((cleanup(x)))

#ifndef NDEBUG
#  define TARCH_DBG 1
#else
#  define TARCH_DBG 0
#endif

#include <stdio.h>
#include <time.h>
#define TARCH_LOG(tag, ...)                                        \
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
#if TARCH_DBG
#  define TARCH_DBG_LOG(tag, ...) TARCH_LOG("debug:" tag, __VA_ARGS__)
#else
#  define TARCH_DBG_LOG(tag, ...)
#endif
