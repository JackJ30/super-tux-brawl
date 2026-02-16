#ifndef GMATH_H_
#define GMATH_H_

#include "inc.h"

typedef struct {
    f32 x, y;
} vec2;

typedef struct {
    f32 x, y, z;
} vec3;

typedef struct {
    f32 data[16];
} mat4;

#endif
