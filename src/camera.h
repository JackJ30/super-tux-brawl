#ifndef CAMERA_H_
#define CAMERA_H_

#include "util/gmath.h"

typedef struct {
    vec2 position;
    float aspect;
    float scale;
} Camera;

vec2 get_aspect_scaler(Camera*);

vec2 screen_to_world(Camera*, vec2 screen);
vec2 world_to_screen(Camera*, vec2 world);
vec2 screen_to_world_vec(Camera*, vec2 screen);
vec2 world_to_screen_vec(Camera*, vec2 world);

mat4 get_ortho_mat(Camera* c);

#endif
