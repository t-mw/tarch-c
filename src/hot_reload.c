#include "hot_reload.h"
#include "game_api.h"

#include <sx/os.h>
#include <tarch/tarch.h>

#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

struct HotReloadContext {
  void* handle;
  ino_t id;
  struct GameApi api;
  struct GameState* state;
};

struct HotReloadContext* hot_reload_context_create(sx_alloc const* alloc)
{
  struct HotReloadContext* context = sx_calloc(alloc, sizeof(struct HotReloadContext));
#if !TARCH_ENABLE_HOTRELOAD
  context->api = GAME_API;
  context->state = GAME_API.create(alloc);
#endif
  return context;
}

bool hot_reload_context_handle_event(struct HotReloadContext* context, void* arg)
{
  return context->api.handle_event(context->state, arg);
}

void hot_reload_context_destroy(sx_alloc const* alloc, struct HotReloadContext* context)
{
#if TARCH_ENABLE_HOTRELOAD
  if (context->handle) {
    context->api.destroy(alloc, context->state);
    context->state = NULL;
    dlclose(context->handle);
    context->handle = NULL;
    context->id = 0;
  }
#else
  context->api.destroy(alloc, context->state);
  context->state = NULL;
#endif
}

/* http://nullprogram.com/blog/2014/12/23/ */
void hot_reload(sx_alloc const* alloc, struct HotReloadContext* context, char* exe_path)
{
#if TARCH_ENABLE_HOTRELOAD
  struct stat attr;
  void* handle;
  struct GameApi* api;

  char game_lib_path[TARCH_MAX_PATH];
  sx_os_path_dirname(game_lib_path, sizeof(game_lib_path), exe_path);
  sx_os_path_join(game_lib_path, sizeof(game_lib_path), game_lib_path, "libgame" SX_DLL_EXT);

  if (stat(game_lib_path, &attr) != 0) {
    fprintf(stderr, "Failed to find dynamic library '%s': %s\n", game_lib_path, strerror(errno));
    abort();
  }

  if (context->id != attr.st_ino) {
    if (context->handle) {
      context->api.unload(context->state);
      dlclose(context->handle);
    }

    handle = dlopen(game_lib_path, RTLD_NOW);

    if (handle) {
      context->handle = handle;
      context->id = attr.st_ino;
      api = dlsym(context->handle, "GAME_API");

      if (api != NULL) {
        context->api = *api;

        if (context->state == NULL) {
          context->state = context->api.create(alloc);
        }

        TARCH_LOG("hot_reload", "Reloading library");
        context->api.reload(context->state);
      } else {
        dlclose(context->handle);
        context->handle = NULL;
        context->id = 0;
      }
    } else {
      fprintf(stderr, "Failed to load dynamic library '%s': %s\n", game_lib_path, dlerror());
      context->handle = NULL;
      context->id = 0;
    }
  }
#else
  sx_unused(alloc);
  sx_unused(context);
  sx_unused(exe_path);
#endif
}
