#include "runner/game_api.h"
#include "runner/host_state.h"

#include <raylib.h>
#include <sokol/sokol_audio.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_time.h>
#include <sx/sx.h>
#include <tarch/tarch.h>

#include <math.h>
#include <stdio.h>

struct GameState {
    float rotation;
};

static void game_state_init(sx_alloc const* alloc, struct GameState* state) {
    sx_unused(alloc);
    state->rotation = 0.0;
}

static void game_state_discard(sx_alloc const* alloc, struct GameState* state) {
    sx_unused(alloc);
    sx_unused(state);
}

static void game_state_handle_event(sx_alloc const* alloc, struct GameState* state, struct Event event) {
    sx_unused(alloc);

    float dt = GetFrameTime();
    if (strcmp(event.type, "update") == 0) {
        state->rotation += dt * 0.7f;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D((Camera3D){
            .position = {sinf(state->rotation) * 10.0f, 10.0f, cosf(state->rotation) * 10.0f},
            .target = {0.0f, 0.0f, 0.0f},
            .up = {0.0f, 1.0f, 0.0f},
            .fovy = 45.0f,
        });
        DrawCube((Vector3){0.0f, 0.0f, 0.0f}, 2.0f, 2.0f, 2.0f, RED);
        EndMode3D();
        EndDrawing();
    }
}

static struct GameState* game_create(sx_alloc const* alloc, struct HostState* host_state) {
    sx_unused(alloc);
    sx_unused(host_state);

    struct GameState* state = sx_calloc(alloc, sizeof(*state));
    game_state_init(alloc, state);
    return state;
}

static void game_destroy(sx_alloc const* alloc, struct GameState* state, struct HostState* host_state) {
    sx_unused(host_state);

    game_state_discard(alloc, state);
    sx_free(alloc, state);
}

static void game_reload(sx_alloc const* alloc, struct GameState* state, struct HostState* host_state) {
    sx_unused(alloc);
    sx_unused(state);
    sx_unused(host_state);
}

static void game_unload(sx_alloc const* alloc, struct GameState* state, struct HostState* host_state) {
    sx_unused(alloc);
    sx_unused(state);
    sx_unused(host_state);
}

static bool game_handle_event(sx_alloc const* alloc, struct GameState* state, struct HostState* host_state, struct Event event) {
    sx_unused(host_state);

    DBG_LOG("game_state", "Handling event: %s", event.type);
    game_state_handle_event(alloc, state, event);

    return true;
}

const struct GameApi GAME_API = {game_create, game_destroy, game_reload, game_unload, game_handle_event};
