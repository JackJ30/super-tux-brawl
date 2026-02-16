#include "config.h"

Config config = {
    .tick_rate = 60,
    .vsync = false,
};

// void config_init(void) {
//
//     byte* shader_code = NULL;
//     size shader_code_size;
//     if (!load_resource_file("config", s.arena, &shader_code, &shader_code_size)) {
//         log_warn("config_init failed to load ");
//         goto end;
//     }
//
// end:
//     release_scratch_arena(s);
// }
