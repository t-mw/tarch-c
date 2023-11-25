#pragma once

#include <stdbool.h>

struct GameState;
struct HostState;

struct Event {
    char* type;
};

struct GameApi {
    /**
     * Returns a fresh game state.
     */
    struct GameState* (*create)(struct HostState* host_state);

    /**
     * Destroys a game state.
     */
    void (*destroy)(struct GameState* state, struct HostState* host_state);

    /**
     * Called exactly once when the game code is reloaded.
     */
    void (*reload)(struct GameState* state, struct HostState* host_state);

    /**
     * Called exactly once when the game code is about to be reloaded.
     */
    void (*unload)(struct GameState* state, struct HostState* host_state);

    /**
     * Called once for each event to be handled.
     * @return true if the program should continue
     */
    bool (*handle_event)(struct GameState* state, struct HostState* host_state, struct Event event);
};

extern const struct GameApi GAME_API;
