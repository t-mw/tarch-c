#define SOKOL_IMPL
#if defined(_MSC_VER)
#  define SOKOL_LOG(str) OutputDebugStringA(str)
#endif

#include <sokol/sokol_app.h>
#include <sokol/sokol_audio.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_time.h>

#include <sokol/util/sokol_shape.h>
