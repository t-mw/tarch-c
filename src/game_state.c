#include "game_api.h"
#include "host_state.h"

#include <sokol/sokol_app.h>
#include <sokol/sokol_audio.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_time.h>
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

static void game_state_handle_event(struct GameState* state, char* event)
{
  sx_unused(state);

  if (strcmp(event, "draw") == 0) {
    int const canvas_width = sapp_width();
    int const canvas_height = sapp_height();
    sg_pass_action const pass_action = { .colors[0] = { .action = SG_ACTION_CLEAR,
                                                        .value = { 1.0f, 0.0f, 0.0f, 1.0f } } };
    sg_begin_default_pass(&pass_action, canvas_width, canvas_height);
    sg_end_pass();
    sg_commit();
  }
}

static struct GameState* game_create(sx_alloc const* alloc, struct HostState* host_state)
{
  sx_unused(host_state);

  struct GameState* state = sx_calloc(alloc, sizeof(*state));
  game_state_init(state);
  return state;
}

static void game_destroy(sx_alloc const* alloc, struct GameState* state,
                         struct HostState* host_state)
{
  sx_unused(host_state);

  game_state_discard(state);
  sx_free(alloc, state);
}

static void game_reload(struct GameState* state, struct HostState* host_state)
{
  sx_unused(state);
  sx_unused(host_state);
}

static void game_unload(struct GameState* state, struct HostState* host_state)
{
  sx_unused(state);
  sx_unused(host_state);
}

static bool game_handle_event(struct GameState* state, void const* arg,
                              struct HostState* host_state)
{
  sx_unused(host_state);

  char* event = (char*)arg;
  TARCH_LOG("game_state", "Handling event: %s", event);
  game_state_handle_event(state, event);

  return true;
}

const struct GameApi GAME_API = { game_create, game_destroy, game_reload, game_unload,
                                  game_handle_event };
