#include "game_api.h"

#include <sx/sx.h>
#include <tarch/tarch.h>

#include <stdio.h>

struct GameState {
};

static void game_state_init(struct GameState* state)
{
  sx_unused(state);
}

static void game_state_discard(struct GameState* state)
{
  sx_unused(state);
}

static struct GameState* game_create(sx_alloc const* alloc)
{
  struct GameState* state = sx_calloc(alloc, sizeof(*state));
  game_state_init(state);
  return state;
}
static void game_destroy(sx_alloc const* alloc, struct GameState* state)
{
  game_state_discard(state);
  sx_free(alloc, state);
}
static void game_reload(struct GameState* state)
{
  sx_unused(state);
}
static void game_unload(struct GameState* state)
{
  sx_unused(state);
}
static bool game_handle_event(struct GameState* state, void const* arg)
{
  sx_unused(state);
  sx_unused(arg);
  TARCH_LOG("game_state", "Handling event: %s", (char*)arg);
  return true;
}

const struct GameApi GAME_API = { game_create, game_destroy, game_reload, game_unload,
                                  game_handle_event };
