#include "sokol_state.h"

#include <sx/math-vec.h>
#include <sx/sx.h>
#include <tarch/tarch.h>

void sokol_state_init(struct SokolState* state) {
    struct sg_pipeline pipeline = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = sg_make_shader(basic_shader_desc(sg_query_backend())),
        .layout = {.buffers[0] = sshape_vertex_buffer_layout_state(),
                   .attrs = {[ATTR_basic_vs_position] = sshape_position_vertex_attr_state(),
                             [ATTR_basic_vs_color0] = sshape_color_vertex_attr_state()}},
        .index_type = SG_INDEXTYPE_UINT16,
        .cull_mode = SG_CULLMODE_NONE,
        .depth = {.compare = SG_COMPAREFUNC_LESS_EQUAL, .write_enabled = true},
    });

    uint32_t tiles = 10;
    struct sshape_sizes_t box_sizes = sshape_box_sizes(tiles);

    struct sshape_vertex_t vertices[box_sizes.vertices.size];
    uint16_t indices[box_sizes.indices.size];

    struct sshape_buffer_t buf = {.vertices =
                                      {
                                          .buffer = SSHAPE_RANGE(vertices),
                                      },
                                  .indices = {
                                      .buffer = SSHAPE_RANGE(indices),
                                  }};

    buf = sshape_build_box(&buf, &(sshape_box_t){
                                     .width = 1.0f,
                                     .height = 1.0f,
                                     .depth = 1.0f,
                                     .tiles = tiles,
                                     .random_colors = true,
                                 });

    struct sg_buffer_desc vbuf_desc = sshape_vertex_buffer_desc(&buf);
    struct sg_buffer vbuf = sg_make_buffer(&vbuf_desc);

    struct sg_buffer_desc ibuf_desc = sshape_index_buffer_desc(&buf);
    struct sg_buffer ibuf = sg_make_buffer(&ibuf_desc);

    state->vbuf = vbuf;
    state->ibuf = ibuf;
    state->pipeline = pipeline;
    state->box_buf = buf;
    state->vs_params = (basic_vs_params_t){0};
    state->rx = 0.0f;
    state->ry = 0.0f;
}

void sokol_state_draw(struct SokolState* state, struct DrawArgs const* args) {
    sx_unused(state);

    int window_width = args->window_width;
    int window_height = args->window_height;

    sx_mat4 proj = sx_mat4_perspectiveFOV(SX_PI / 3.0f, (float)window_width / (float)window_height, 0.01f, 10.0f, true);
    sx_mat4 view = sx_mat4_view_lookat(sx_vec3f(0.0f, 1.5f, 6.0f), sx_vec3f(0.0f, 0.0f, 0.0f), sx_vec3f(0.0f, 1.0f, 0.0f));
    sx_mat4 view_proj = sx_mat4_mul(&proj, &view);

    sx_mat4 rxm = sx_mat4_rotateX(state->rx);
    sx_mat4 rym = sx_mat4_rotateY(state->ry);
    sx_mat4 model = sx_mat4_mul(&rxm, &rym);

    view_proj = sx_mat4_mul(&view_proj, &model);

    memcpy(&state->vs_params.mvp, &view_proj.f, sizeof(state->vs_params.mvp));

    struct sg_pass_action pass_action = {.colors[0] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {0.0f, 0.0f, 0.0f, 1.0f}}};

    sg_begin_default_pass(&pass_action, window_width, window_height);
    sg_apply_pipeline(state->pipeline);
    sg_apply_bindings(&(sg_bindings){.vertex_buffers[0] = state->vbuf, .index_buffer = state->ibuf});
    sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_basic_vs_params, &SG_RANGE(state->vs_params));

    struct sshape_element_range_t elms = sshape_element_range(&state->box_buf);
    sg_draw(elms.base_element, elms.num_elements, 1);

    sg_end_pass();
    sg_commit();
}
