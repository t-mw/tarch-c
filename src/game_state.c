#include "runner/game_api.h"
#include "runner/host_state.h"

#include <raylib.h>
#include <tarch/tarch.h>

#include <math.h>
#include <stdio.h>
#include <string.h>

struct GameState {
    float rotation;
};

static void game_state_init(struct GameState* state) {
    state->rotation = 0.0;
}

static void game_state_discard(struct GameState* state) {
    tm_unused(state);
}

static void game_state_handle_event(struct GameState* state, struct Event event) {
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

static struct GameState* game_create(struct HostState* host_state) {
    tm_unused(host_state);

    struct GameState* state = tm_calloc(1, sizeof(*state));
    game_state_init(state);
    return state;
}

static void game_destroy(struct GameState* state, struct HostState* host_state) {
    tm_unused(host_state);

    game_state_discard(state);
    tm_free(state);
}

static void game_reload(struct GameState* state, struct HostState* host_state) {
    tm_unused(state);
    tm_unused(host_state);
}

static void game_unload(struct GameState* state, struct HostState* host_state) {
    tm_unused(state);
    tm_unused(host_state);
}

static bool game_handle_event(struct GameState* state, struct HostState* host_state, struct Event event) {
    tm_unused(host_state);

    DBG_LOG("game_state", "Handling event: %s", event.type);
    game_state_handle_event(state, event);

    return true;
}

const struct GameApi GAME_API = {game_create, game_destroy, game_reload, game_unload, game_handle_event};
