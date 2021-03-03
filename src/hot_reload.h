#pragma once

#include <sx/allocator.h>

struct HotReloadContext;
struct HotReloadContext* hot_reload_context_create(sx_alloc const* alloc);
bool hot_reload_context_handle_event(struct HotReloadContext* context, void* arg);
void hot_reload_context_destroy(sx_alloc const* alloc, struct HotReloadContext* context);
void hot_reload(sx_alloc const* alloc, struct HotReloadContext* context, char* exe_path);
