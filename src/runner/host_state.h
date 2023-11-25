#pragma once

struct HostState {
    // imgui context etc.
};

struct HostState* host_state_create();
void host_state_destroy(struct HostState* state);
