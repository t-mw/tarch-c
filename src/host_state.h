#pragma once

#include <sx/allocator.h>

struct HostState {
  // imgui context etc.
};

struct HostState* host_state_create(sx_alloc const* alloc);
void host_state_destroy(sx_alloc const* alloc, struct HostState* state);
