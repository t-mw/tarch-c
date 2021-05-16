#include "game_api.h"
#include "host_state.h"
#include "sokol_state.h"

#include <sokol/sokol_app.h>
#include <sokol/sokol_audio.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_time.h>
#include <sx/sx.h>
#include <tarch/tarch.h>

#include <stdio.h>

struct GameState {
  bool is_sokol_initialized;
  struct SokolState sokol_state;
};

static void game_state_init(sx_alloc const* alloc, struct GameState* state)
{
  sx_unused(alloc);
  state->is_sokol_initialized = false;
}

static void game_state_discard(sx_alloc const* alloc, struct GameState* state)
{
  sx_unused(alloc);
  sx_unused(state);
}

static void game_state_handle_event(sx_alloc const* alloc, struct GameState* state, char* event)
{
  sx_unused(alloc);

  if (strcmp(event, "frame") == 0) {
    state->sokol_state.rx += 0.01f;
    state->sokol_state.ry += 0.02f;
  }
  if (strcmp(event, "init_draw") == 0 && !state->is_sokol_initialized) {
    sokol_state_init(&state->sokol_state);
    state->is_sokol_initialized = true;
  } else if (strcmp(event, "draw") == 0) {
    sokol_state_draw(&state->sokol_state);
  }
}

static struct GameState* game_create(sx_alloc const* alloc, struct HostState* host_state)
{
  sx_unused(alloc);
  sx_unused(host_state);

  struct GameState* state = sx_calloc(alloc, sizeof(*state));
  game_state_init(alloc, state);
  return state;
}

static void game_destroy(sx_alloc const* alloc, struct GameState* state,
                         struct HostState* host_state)
{
  sx_unused(host_state);

  game_state_discard(alloc, state);
  sx_free(alloc, state);
}

static void game_reload(sx_alloc const* alloc, struct GameState* state,
                        struct HostState* host_state)
{
  sx_unused(alloc);
  sx_unused(state);
  sx_unused(host_state);
}

static void game_unload(sx_alloc const* alloc, struct GameState* state,
                        struct HostState* host_state)
{
  sx_unused(alloc);
  sx_unused(state);
  sx_unused(host_state);
}

static bool game_handle_event(sx_alloc const* alloc, struct GameState* state,
                              struct HostState* host_state, void const* arg)
{
  sx_unused(host_state);

  char* event = (char*)arg;
  TARCH_DBG_LOG("game_state", "Handling event: %s", event);
  game_state_handle_event(alloc, state, event);

  return true;
}

const struct GameApi GAME_API = { game_create, game_destroy, game_reload, game_unload,
                                  game_handle_event };
