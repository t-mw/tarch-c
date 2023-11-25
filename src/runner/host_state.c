#include "host_state.h"

#include <tarch/tarch.h>

struct HostState* host_state_create() {
    struct HostState* state = tarch_calloc(1, sizeof(*state));
    return state;
}

void host_state_destroy(struct HostState* state) {
    tarch_free(state);
}
