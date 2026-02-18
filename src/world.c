#include "world.h"

State state = {0};

void world_init() {
    state.guys = da_create(Guy, 2);
    state.guys[0] = (Guy){ .position = (vec2){ 0.0f, 0.0f }};
    state.guys[1] = (Guy){ .position = (vec2){ -0.7f, 0.3f }};
    state.owned_guy = 0;
}

void world_shutdown() {
    da_destroy(state.guys);
}

State* world_sim(f32 dt, Input* input) {

    if (input->swap) state.owned_guy = (state.owned_guy + 1) % da_size(state.guys);

    /* update guys */
    for (size i = 0; i < da_size(state.guys); ++i) {
        Guy* g = &state.guys[i];

        // handle player input
        if (i == state.owned_guy) {
            if (input->jump && g->position.y == -1.0f) {
                g->velocity.y = 5.0f;
            }
            g->velocity.x = input->direction.x * 2.0f;
        }

        // semi implicit euler integrator
        g->velocity.y -= 9.8f * dt;
        g->position.y += g->velocity.y * dt;
        g->position.x += g->velocity.x * dt;

        // handle ground
        if (g->position.y < -1.0f) {
            g->position.y = -1.0f;
        }
    }

    return &state;
}
