#include "hot_reload.h"

#include <sokol/sokol_app.h>
#include <sokol/sokol_audio.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_time.h>

#include <stdio.h>

static void init(void);
static void frame(void);
static void cleanup(void);

static char** g_argv;
static struct AppState {
  struct HotReloadContext* hot_reload_context;
} app_state = {};

sapp_desc sokol_main(int argc, char* argv[])
{
  sx_unused(argc);
  g_argv = argv;

  return (sapp_desc){ .init_cb = init,
                      .frame_cb = frame,
                      .cleanup_cb = cleanup,
                      .width = 1024,
                      .height = 768,
                      .window_title = "tarch" };
}

static void init(void)
{
  app_state.hot_reload_context = hot_reload_context_create(sx_alloc_malloc());
  sg_setup(&(sg_desc){ .context = sapp_sgcontext() });
}

static void frame(void)
{
  char* exe_path = g_argv[0];
  hot_reload(sx_alloc_malloc(), app_state.hot_reload_context, exe_path);
  hot_reload_context_handle_event(app_state.hot_reload_context, "frame");

  int const canvas_width = sapp_width();
  int const canvas_height = sapp_height();
  sg_pass_action const pass_action = { .colors[0] = { .action = SG_ACTION_CLEAR,
                                                      .value = { 1.0f, 0.0f, 0.0f, 1.0f } } };
  sg_begin_default_pass(&pass_action, canvas_width, canvas_height);
  sg_end_pass();
  sg_commit();
}

static void cleanup(void)
{
  sg_shutdown();

  sx_free(sx_alloc_malloc(), app_state.hot_reload_context);
  app_state.hot_reload_context = NULL;
}
