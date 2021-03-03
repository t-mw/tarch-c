#include "host_state.h"
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

static struct HostState* host_state;
static struct HotReloadContext* hot_reload_context;

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
  host_state = host_state_create(sx_alloc_malloc());
  hot_reload_context = hot_reload_context_create(sx_alloc_malloc(), host_state);

  sg_setup(&(sg_desc){ .context = sapp_sgcontext() });
}

static void frame(void)
{
  char* exe_path = g_argv[0];
  hot_reload(sx_alloc_malloc(), hot_reload_context, host_state, exe_path);
  hot_reload_context_handle_event(hot_reload_context, "frame", host_state);

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

  hot_reload_context_destroy(sx_alloc_malloc(), hot_reload_context, host_state);
  hot_reload_context = NULL;

  host_state_destroy(sx_alloc_malloc(), host_state);
  host_state = NULL;
}
