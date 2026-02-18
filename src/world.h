#ifndef WORLD_H_
#define WORLD_H_

#include "util/inc.h"
#include "util/gmath.h"
#include "util/da.h"
#include "input.h"

typedef struct {
    vec2 position;
    vec2 velocity;
} Guy;

typedef struct {
    DA(Guy) guys;
    size owned_guy;
} State;

void world_init();
void world_shutdown();

State* world_sim(f32 dt, Input* input);

#endif
