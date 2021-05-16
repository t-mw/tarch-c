// from https://github.com/floooh/pacman.c/
#define SOKOL_IMPL
#if defined(_MSC_VER)
#  define SOKOL_D3D11
#  define SOKOL_LOG(str) OutputDebugStringA(str)
#elif defined(__EMSCRIPTEN__)
#  define SOKOL_GLES2
#elif defined(__APPLE__)
// NOTE: on macOS, sokol.c is compiled explicitly as ObjC
#  define SOKOL_METAL
#else
#  define SOKOL_GLCORE33
#endif
#include <sokol/sokol_app.h>
#include <sokol/sokol_audio.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_time.h>
