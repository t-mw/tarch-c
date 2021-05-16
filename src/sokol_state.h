#include <sokol/sokol_gfx.h>
#include <sokol/util/sokol_shape.h>

#include "../shaders/basic.glsl.h"

struct SokolState {
  struct sg_buffer vbuf;
  struct sg_buffer ibuf;
  struct sg_pipeline pipeline;
  struct sshape_buffer_t box_buf;
  struct basic_vs_params_t vs_params;
  float rx;
  float ry;
};

void sokol_state_init(struct SokolState* state);
void sokol_state_draw(struct SokolState* state);
