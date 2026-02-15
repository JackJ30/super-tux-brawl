#include "world.h"

#include "da.h"

State state = {0};

void world_init() {
    state.entities = da_create(Entity, 2);
    state.entities[0] = (Entity){ .position = (Vec2){ 0.0f, 0.0f }};
    state.entities[1] = (Entity){ .position = (Vec2){ -0.7f, 0.3f }};
    state.owned_entity = 0;
}

void world_shutdown() {
    da_destroy(state.entities);
}

State* world_sim(f32 dt, Input* input) {

    for (size i = 0; i < da_size(state.entities); ++i) {
        Entity* e = &state.entities[i];

        // handle player input
        if (i == state.owned_entity) {
            if (input->jump && e->position.y == -1.0f) {
                e->velocity.y = 5.0f;
            }
            e->velocity.x = input->direction.x * 2.0f;
        }

        // semi implicit euler integrator
        e->velocity.y -= 9.8f * dt;
        e->position.y += e->velocity.y * dt;
        e->position.x += e->velocity.x * dt;

        // handle ground
        if (e->position.y < -1.0f) {
            e->position.y = -1.0f;
        }
    }

    return &state;
}
