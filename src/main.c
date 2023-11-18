#include "tarch/host_state.h"
#include "tarch/hot_reload.h"

#include <sokol/sokol_audio.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_log.h>
#include <sokol/sokol_time.h>
#include <tarch/tarch.h>

#include <SDL.h>
#include <stdio.h>

static char* g_exe_path;

static struct HostState* host_state;
static struct HotReloadContext* hot_reload_context;

struct EventArgs {
    double dt;
    int window_width;
    int window_height;
};

static bool handle_events(struct EventArgs const* args) {
    struct Event event = {.dt = args->dt, .window_width = args->window_width, .window_height = args->window_height};

    // setup gfx resources in runner binary context until
    // https://github.com/floooh/sokol/issues/91 is implemented
    event.type = "init_draw";
    if (!GAME_API.handle_event(sx_alloc_malloc(), hot_reload_context_get_game_state(hot_reload_context), host_state, event)) {
        return false;
    }

    event.type = "update";
    if (!hot_reload_context_handle_event(sx_alloc_malloc(), hot_reload_context, host_state, event)) {
        return false;
    }

    // run the draw action in the runner binary context until
    // https://github.com/floooh/sokol/issues/91 is implemented
    event.type = "draw";
    if (!GAME_API.handle_event(sx_alloc_malloc(), hot_reload_context_get_game_state(hot_reload_context), host_state, event)) {
        return false;
    }

    return true;
}

int main(int argc, char** argv) {
    sx_unused(argc);

    g_exe_path = argv[0];

    DBG_LOG("main", "Initializing app");

    host_state = host_state_create(sx_alloc_malloc());
    hot_reload_context = hot_reload_context_create(sx_alloc_malloc(), host_state);

    stm_setup();

    // create window and GL context via SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_Window* window = SDL_CreateWindow("tarch", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (SDL_GL_SetSwapInterval(-1) == -1) {
        // adaptive vsync not supported, use normal vsync
        SDL_GL_SetSwapInterval(1);
    }

    sg_setup(&(sg_desc){
        .logger.func = slog_func,
    });

    bool running = true;
    while (running) {
        hot_reload(sx_alloc_malloc(), hot_reload_context, host_state, g_exe_path);

        static uint64_t last_time = 0;
        if (!last_time) {
            last_time = stm_now();
        }
        double dt = stm_sec(stm_laptime(&last_time));

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
        }

        int window_width, window_height;
        SDL_GL_GetDrawableSize(window, &window_width, &window_height);

        struct EventArgs args = {.dt = dt, .window_width = window_width, .window_height = window_height};
        if (!handle_events(&args)) {
            running = false;
        }

        SDL_GL_SwapWindow(window);
    }

    DBG_LOG("main", "Exiting app");

    sg_shutdown();

    hot_reload_context_destroy(sx_alloc_malloc(), hot_reload_context, host_state);
    hot_reload_context = NULL;

    host_state_destroy(sx_alloc_malloc(), host_state);
    host_state = NULL;

    // clean up SDL
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
