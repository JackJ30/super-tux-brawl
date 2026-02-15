#ifndef VEC_H_
#define VEC_H_

#include "inc.h"

typedef struct {
    f32 x, y;
} Vec2;

typedef struct {
    f32 x, y, z;
} Vec3;

typedef struct {
    f32 data[16];
} Mat4;

#endif
