#include "state.h"

/* void world_init() { */
/*     state.guys = da_create(Guy, 2); */
/*     state.guys[0] = (Guy){ .position = (vec2){ 0.0f, 0.0f }}; */
/*     state.guys[1] = (Guy){ .position = (vec2){ -0.7f, 0.3f }}; */
/*     state.owned_guy = 0; */
/* } */

/* void world_shutdown() { */
/*     da_destroy(state.guys); */
/* } */

void sim_tick(State* s, f32 dt) {

    /* update guys */
    for (size i = 0; i < da_size(s->guys); ++i) {
        Guy* g = &s->guys[i];

        // handle player input
        /* if (i == s->owned_guy) { */
        /*     if (input->jump && g->position.y == -1.0f) { */
        /*         g->velocity.y = 5.0f; */
        /*     } */
        /*     g->velocity.x = input->direction.x * 2.0f; */
        /* } */

        // semi implicit euler integrator
        g->velocity.y -= 9.8f * dt;
        g->position.y += g->velocity.y * dt;
        g->position.x += g->velocity.x * dt;

        // handle ground
        if (g->position.y < -1.0f) {
            g->position.y = -1.0f;
        }
    }
}

GuyHandle create_guy(State* s) {
    int index = da_size(s->guys);
    da_add(s->guys, (Guy){ .exists = true });
    return (GuyHandle){ .index = index, .generation = s->guys[index].generation};
}
