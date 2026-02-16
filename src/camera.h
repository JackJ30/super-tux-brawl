#ifndef CAMERA_H_
#define CAMERA_H_

#include "util/vec.h"

typedef struct {
    Vec2 position;
    float aspect;
    float scale;
} Camera;

Vec2 get_aspect_scaler(Camera*);

Vec2 screen_to_world(Camera*, Vec2 screen);
Vec2 world_to_screen(Camera*, Vec2 world);
Vec2 screen_to_world_vec(Camera*, Vec2 screen);
Vec2 world_to_screen_vec(Camera*, Vec2 world);

Mat4 get_ortho_mat(Camera* c);

#endif
