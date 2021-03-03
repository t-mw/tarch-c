#pragma once

#include "game_api.h"

#include <sokol/sokol_gfx.h>
#include <sx/allocator.h>

struct HotReloadContext;

struct GameState* hot_reload_context_get_game_state(struct HotReloadContext* context);

struct HotReloadContext* hot_reload_context_create(sx_alloc const* alloc,
                                                   struct HostState* host_state);
bool hot_reload_context_handle_event(struct HotReloadContext* context, void* arg,
                                     struct HostState* host_state);
void hot_reload_context_destroy(sx_alloc const* alloc, struct HotReloadContext* context,
                                struct HostState* host_state);
void hot_reload(sx_alloc const* alloc, struct HotReloadContext* context,
                struct HostState* host_state, char* exe_path);
