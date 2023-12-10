#include "runner/host_state.h"
#include "runner/hot_reload.h"

#include <raylib.h>
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
    tm_unused(argc);

    g_exe_path = argv[0];

    debug_log("main", "Initializing app");

    InitWindow(640, 480, "example");
    SetTargetFPS(60);

    host_state = host_state_create();
    hot_reload_context = hot_reload_context_create(host_state);

    while (!WindowShouldClose()) {
        hot_reload(hot_reload_context, host_state, g_exe_path);

        struct Event event = {.type = "update"};
        if (!hot_reload_context_handle_event(hot_reload_context, host_state, event)) {
            break;
        }
    }

    debug_log("main", "Exiting app");

    hot_reload_context_destroy(hot_reload_context, host_state);
    hot_reload_context = NULL;

    host_state_destroy(host_state);
    host_state = NULL;

    CloseWindow();

    return 0;
}
