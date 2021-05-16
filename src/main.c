#include "tarch/host_state.h"
#include "tarch/hot_reload.h"

#include <sokol/sokol_app.h>
#include <sokol/sokol_audio.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_time.h>
#include <tarch/tarch.h>

#include <stdio.h>

static void init(void);
static void frame(void);
static void cleanup(void);

static char* g_exe_path;

static struct HostState* host_state;
static struct HotReloadContext* hot_reload_context;

sapp_desc sokol_main(int argc, char* argv[])
{
  sx_unused(argc);
  g_exe_path = argv[0];

  return (sapp_desc){ .init_cb = init,
                      .frame_cb = frame,
                      .cleanup_cb = cleanup,
                      .width = 1024,
                      .height = 768,
                      .window_title = "tarch" };
}

static void init(void)
{
  stm_setup();
  sg_setup(&(sg_desc){ .context = sapp_sgcontext() });

  host_state = host_state_create(sx_alloc_malloc());
  hot_reload_context = hot_reload_context_create(sx_alloc_malloc(), host_state);
}

static void frame(void)
{
  static uint64_t last_time = 0;
  if (!last_time) {
    last_time = stm_now();
  }
  double dt = stm_sec(stm_laptime(&last_time));

  hot_reload(sx_alloc_malloc(), hot_reload_context, host_state, g_exe_path);

  // setup gfx resources in runner binary context until
  // https://github.com/floooh/sokol/issues/91 is implemented
  GAME_API.handle_event(sx_alloc_malloc(), hot_reload_context_get_game_state(hot_reload_context),
                        host_state, (struct Event){ .type = "init_draw", .dt = dt });

  hot_reload_context_handle_event(sx_alloc_malloc(), hot_reload_context, host_state,
                                  (struct Event){ .type = "update", .dt = dt });

  // run the draw action in the runner binary context until
  // https://github.com/floooh/sokol/issues/91 is implemented
  GAME_API.handle_event(sx_alloc_malloc(), hot_reload_context_get_game_state(hot_reload_context),
                        host_state, (struct Event){ .type = "draw", .dt = dt });
}

static void cleanup(void)
{
  sg_shutdown();

  hot_reload_context_destroy(sx_alloc_malloc(), hot_reload_context, host_state);
  hot_reload_context = NULL;

  host_state_destroy(sx_alloc_malloc(), host_state);
  host_state = NULL;
}
