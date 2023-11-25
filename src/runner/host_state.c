#include "host_state.h"

struct HostState* host_state_create(sx_alloc const* alloc) {
    struct HostState* state = sx_calloc(alloc, sizeof(*state));
    return state;
}

void host_state_destroy(sx_alloc const* alloc, struct HostState* state) {
    sx_free(alloc, state);
}
