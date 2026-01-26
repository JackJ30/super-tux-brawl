#ifndef WORLD_H_
#define WORLD_H_

#include "inc.h"
#include "vec.h"

typedef struct {
    Vec2 position;
    Vec2 velocity;
} Entity;

typedef struct {
    Entity* entities; // da
} State;

void world_init();
void world_shutdown();

State* world_sim(f32 dt);

#endif
