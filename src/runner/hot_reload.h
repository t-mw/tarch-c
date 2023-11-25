#pragma once

#include "game_api.h"

struct HotReloadContext;

struct GameState* hot_reload_context_get_game_state(struct HotReloadContext* context);

struct HotReloadContext* hot_reload_context_create(struct HostState* host_state);
bool hot_reload_context_handle_event(struct HotReloadContext* context, struct HostState* host_state, struct Event event);
void hot_reload_context_destroy(struct HotReloadContext* context, struct HostState* host_state);
void hot_reload(struct HotReloadContext* context, struct HostState* host_state, char* exe_path);
