#include "runner/game_api.h"
#include "runner/host_state.h"

#include <raylib.h>
#include <raymath.h>
#include <tarch/tarch.h>

#include <math.h>
#include <stdio.h>
#include <string.h>

#define RLIGHTS_IMPLEMENTATION
#include <raylib/examples/shaders/rlights.h>

struct GameState {
    Model model;
    Shader shader;
    float rotation;
};

static void game_state_init(struct GameState* state) {
    Shader shader = LoadShader(ASSETS_DIR "shaders/lighting.vs", ASSETS_DIR "shaders/lighting.fs");
    Mesh cube = GenMeshCube(2.0f, 2.0f, 2.0f);
    Model model = LoadModelFromMesh(cube);
    model.materials[0].shader = shader;

    state->model = model;
    state->shader = shader;
    state->rotation = 0.0;
}

static void game_state_discard(struct GameState* state) {
    UnloadModel(state->model);
    UnloadShader(state->shader);
}

static void game_state_handle_event(struct GameState* state, struct Event event) {
    float dt = GetFrameTime();
    if (strcmp(event.type, "update") == 0) {
        state->rotation += dt * 0.7f;

        Vector3 camera_pos = {sinf(state->rotation) * 10.0f, 10.0f, cosf(state->rotation) * 10.0f};

        Shader shader = state->shader;
        Light lights[MAX_LIGHTS] = {0};
        lights[0] = CreateLight(LIGHT_POINT, (Vector3){-3, 4, -3}, Vector3Zero(), YELLOW, shader);
        lights[1] = CreateLight(LIGHT_POINT, (Vector3){3, 3, 3}, Vector3Zero(), RED, shader);
        lights[2] = CreateLight(LIGHT_POINT, (Vector3){-3, 1, 3}, Vector3Zero(), GREEN, shader);
        lights[3] = CreateLight(LIGHT_POINT, (Vector3){3, 1, -3}, Vector3Zero(), BLUE, shader);
        SetShaderValue(state->shader, state->shader.locs[SHADER_LOC_VECTOR_VIEW], Vector3ToFloat(camera_pos), SHADER_UNIFORM_VEC3);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D((Camera3D){
            .position = camera_pos,
            .target = {0.0f, 0.0f, 0.0f},
            .up = {0.0f, 1.0f, 0.0f},
            .fovy = 45.0f,
        });
        DrawModel(state->model, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
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

    DEBUG_LOG("game_state", "Handling event: %s", event.type);
    game_state_handle_event(state, event);

    return true;
}

const struct GameApi GAME_API = {game_create, game_destroy, game_reload, game_unload, game_handle_event};
