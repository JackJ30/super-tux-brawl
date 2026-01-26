#include "world.h"

#include "da.h"

State state = {0};

void world_init() {
    state.entities = da_create(Entity, 3);
    state.entities[0] = (Entity){ .position = (Vec2){ .x = 0.0f, .y = 0.0f }  };
    state.entities[1] = (Entity){ .position = (Vec2){ .x = -0.5f, .y = 0.3f } };
    state.entities[2] = (Entity){ .position = (Vec2){ .x = 0.5f, .y = -1.0f } };
}

void world_shutdown() {
    da_destroy(state.entities);
}

State* world_sim(f32 dt) {

    da_for_all(Entity, e, state.entities) {
        e->velocity.y -= 0.01f;
        e->position.y += e->velocity.y;
        if (e->position.y < -1.0f) {
            e->position.y = -1.0f;
            e->velocity.y = -e->velocity.y * 0.8f;
        }
    }

    return &state;
}
