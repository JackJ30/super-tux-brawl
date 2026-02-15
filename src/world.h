#ifndef WORLD_H_
#define WORLD_H_

#include "inc.h"
#include "input.h"
#include "vec.h"

typedef struct {
    Vec2 position;
    Vec2 velocity;
} Entity;

typedef struct {
    Entity* entities; // da
    size owned_entity;
} State;

void world_init();
void world_shutdown();

State* world_sim(f32 dt, Input* input);

#endif
