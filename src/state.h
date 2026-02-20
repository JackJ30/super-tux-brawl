#ifndef STATE_H_
#define STATE_H_

#include "util/inc.h"
#include "util/gmath.h"
#include "util/da.h"
/* #include "input.h" */

typedef struct {
    vec2 position;
    vec2 velocity;
    int generation;
    b8 exists;
} Guy;

typedef struct {
    int index;
    int generation;
} GuyHandle;

typedef struct {
    DA(Guy) guys;
} State;

void sim_tick(State* s, f32 dt);
GuyHandle create_guy(State* s);

#endif
