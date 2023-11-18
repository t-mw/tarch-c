#pragma once

#include <sx/allocator.h>

#include <stdbool.h>

struct GameState;
struct HostState;

struct Event {
    char* type;
    double dt;
    int window_width;
    int window_height;
};

struct GameApi {
    /**
     * Returns a fresh game state.
     */
    struct GameState* (*create)(sx_alloc const* alloc, struct HostState* host_state);

    /**
     * Destroys a game state.
     */
    void (*destroy)(sx_alloc const* alloc, struct GameState* state, struct HostState* host_state);

    /**
     * Called exactly once when the game code is reloaded.
     */
    void (*reload)(sx_alloc const* alloc, struct GameState* state, struct HostState* host_state);

    /**
     * Called exactly once when the game code is about to be reloaded.
     */
    void (*unload)(sx_alloc const* alloc, struct GameState* state, struct HostState* host_state);

    /**
     * Called once for each event to be handled.
     * @return true if the program should continue
     */
    bool (*handle_event)(sx_alloc const* alloc, struct GameState* state, struct HostState* host_state, struct Event event);
};

extern const struct GameApi GAME_API;
