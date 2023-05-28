#pragma once

#include <assert.h>
#include <stdbool.h>

#define _OPTION_PASTE(a, b) a##b
#define OPTION_PASTE(a, b) _OPTION_PASTE(a, b)

#define OPTION(a) OPTION_PASTE(__option_type_, a)

#define NEW_OPTION_TYPE(a) \
    typedef struct {       \
        a __v;             \
        bool __is_some;    \
    } OPTION(a);

#define option_some(v) \
    { .__v = (v), .__is_some = true }

#define option_none() \
    { .__v = 0, .__is_some = false }

#define option_is_some(o) ((o).__is_some)
#define option_is_none(o) (!(o).__is_some)

#define option_unwrap(o)                      \
    ({                                        \
        typeof(o) __option_tmp = (o);         \
        assert(option_is_some(__option_tmp)); \
        __option_tmp.__v;                     \
    })

#define option_unwrap_or(o, def)                           \
    ({                                                     \
        typeof(o) __option_tmp = (o);                      \
        (__option_tmp.__is_some) ? __option_tmp.__v : def; \
    })

#define option_map(o, f)                 \
    ({                                   \
        typeof(o) __option_tmp = (o);    \
        if (__option_tmp.__is_some) {    \
            __option_tmp.__v = f(o.__v); \
        }                                \
        __option_tmp;                    \
    })

#define option_equals(a, b)                                                                                   \
    ({                                                                                                        \
        typeof(a) __option_tmp_a = (a);                                                                       \
        typeof(b) __option_tmp_b = (b);                                                                       \
        (__option_tmp_a.__is_some ? (__option_tmp_b.__is_some ? __option_tmp_a.__v == __option_tmp_b.__v : 0) \
                                  : !__option_tmp_b.__is_some);                                               \
    })
