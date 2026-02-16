#ifndef WORLD_H_
#define WORLD_H_

#include "util/inc.h"
#include "util/vec.h"
#include "input.h"

typedef struct {
    Vec2 position;
    Vec2 velocity;
} Guy;

typedef struct {
    Guy* guys; // da
    size owned_guy;
} State;

void world_init();
void world_shutdown();

State* world_sim(f32 dt, Input* input);

#endif
