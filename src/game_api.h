#pragma once

#include <sx/allocator.h>

#include <stdbool.h>

struct GameState;

struct GameApi {
  /**
   * Returns a fresh game state.
   */
  struct GameState* (*create)(sx_alloc const* alloc);

  /**
   * Destroys a game state.
   */
  void (*destroy)(sx_alloc const* alloc, struct GameState* state);

  /**
   * Called exactly once when the game code is reloaded.
   */
  void (*reload)(struct GameState* state);

  /**
   * Called exactly once when the game code is about to be reloaded.
   */
  void (*unload)(struct GameState* state);

  /**
   * Called once for each event to be handled.
   * @return true if the program should continue
   */
  bool (*handle_event)(struct GameState* state, void const* arg);
};

extern const struct GameApi GAME_API;
