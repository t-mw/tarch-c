#include "hot_reload.h"

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

struct GameState* hot_reload_context_get_game_state(struct HotReloadContext* context) {
    return context->state;
}

struct HotReloadContext* hot_reload_context_create(struct HostState* host_state) {
    struct HotReloadContext* context = tm_calloc(1, sizeof(struct HotReloadContext));
#if ENABLE_HOTRELOAD
    tm_unused(host_state);
#else
    context->api = GAME_API;
    context->state = GAME_API.create(host_state);
#endif
    return context;
}

bool hot_reload_context_handle_event(struct HotReloadContext* context, struct HostState* host_state, struct Event event) {
    return context->api.handle_event(context->state, host_state, event);
}

void hot_reload_context_destroy(struct HotReloadContext* context, struct HostState* host_state) {
#if ENABLE_HOTRELOAD
    if (context->handle) {
        context->api.destroy(context->state, host_state);
        context->state = NULL;
        dlclose(context->handle);
        context->handle = NULL;
        context->id = 0;
    }
#else
    context->api.destroy(context->state, host_state);
    context->state = NULL;
#endif
    tm_free(context);
}

/* http://nullprogram.com/blog/2014/12/23/ */
void hot_reload(struct HotReloadContext* context, struct HostState* host_state, char* exe_path) {
#if ENABLE_HOTRELOAD
    struct stat attr;
    void* handle;
    struct GameApi* api;

    char game_lib_path[256];
    sx_os_path_dirname(game_lib_path, sizeof(game_lib_path), exe_path);
    sx_os_path_join(game_lib_path, sizeof(game_lib_path), game_lib_path, "libgame.so");

    if (stat(game_lib_path, &attr) != 0) {
        fprintf(stderr, "Failed to find dynamic library '%s': %s\n", game_lib_path, strerror(errno));
        abort();
    }

    if (context->id != attr.st_ino) {
        if (context->handle) {
            context->api.unload(context->state, host_state);
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
                    context->state = context->api.create(host_state);
                }

                DBG_LOG("hot_reload", "Reloading library");
                context->api.reload(context->state, host_state);
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
    tm_unused(context);
    tm_unused(host_state);
    tm_unused(exe_path);
#endif
}
