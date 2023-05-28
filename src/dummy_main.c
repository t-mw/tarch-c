#include <sokol/sokol_app.h>

// function to satisfy linker when linking game library for hot reloading
sapp_desc sokol_main(int argc, char* argv[]) {
    (void)(argc);
    (void)(argv);
    return (sapp_desc){};
}
