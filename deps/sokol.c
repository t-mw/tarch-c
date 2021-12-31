#define SOKOL_IMPL
#if defined(_MSC_VER)
#  define SOKOL_LOG(str) OutputDebugStringA(str)
#endif

#include <sx/allocator.h>
#define SOKOL_MALLOC(s) sx_malloc(sx_alloc_malloc(), s)
#define SOKOL_CALLOC(n, s) sx_calloc(sx_alloc_malloc(), n* s)
#define SOKOL_REALLOC(p, s) sx_realloc(sx_alloc_malloc(), p, s)
#define SOKOL_FREE(p) sx_free(sx_alloc_malloc(), p)

#include <sokol/sokol_app.h>
#include <sokol/sokol_audio.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_time.h>

#include <sokol/util/sokol_shape.h>
