#include "runner/host_state.h"
#include "runner/hot_reload.h"

#include <raylib.h>
#include <sokol/sokol_audio.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_log.h>
#include <sokol/sokol_time.h>
#include <tarch/tarch.h>

#include <stdio.h>

static char* g_exe_path;

static struct HostState* host_state;
static struct HotReloadContext* hot_reload_context;

struct EventArgs {
    double dt;
    int window_width;
    int window_height;
};

int main(int argc, char** argv) {
    sx_unused(argc);
    g_exe_path = argv[0];

    DBG_LOG("main", "Initializing app");

    host_state = host_state_create(sx_alloc_malloc());
    hot_reload_context = hot_reload_context_create(sx_alloc_malloc(), host_state);

    InitWindow(640, 480, "example");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        hot_reload(sx_alloc_malloc(), hot_reload_context, host_state, g_exe_path);

        struct Event event = {.type = "update"};
        if (!hot_reload_context_handle_event(sx_alloc_malloc(), hot_reload_context, host_state, event)) {
            break;
        }
    }

    DBG_LOG("main", "Exiting app");

    hot_reload_context_destroy(sx_alloc_malloc(), hot_reload_context, host_state);
    hot_reload_context = NULL;

    host_state_destroy(sx_alloc_malloc(), host_state);
    host_state = NULL;

    CloseWindow();

    return 0;
}
