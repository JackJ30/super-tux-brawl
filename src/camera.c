#include "camera.h"

Vec2 get_aspect_scaler(Camera* c) {
    if (c->aspect >= 1.0f) {
        return (Vec2){ c->scale * c->aspect, c->scale };
    } else {
        return (Vec2){ c->scale, c->scale * (1.0f / c->aspect)};
    }
}

Vec2 screen_to_world(Camera* c, Vec2 screen) {
    Vec2 scaler = get_aspect_scaler(c);
    screen.x = c->position.x + ((screen.x - 0.5f) * scaler.x);
    screen.y = c->position.y + ((0.5f - screen.y) * scaler.y);
    return screen;
}

Vec2 world_to_screen(Camera* c, Vec2 world) {
    Vec2 scaler = get_aspect_scaler(c);
    world.x = ((world.x - c->position.x) / scaler.x) + 0.5f;
    world.y = 0.5f - ((world.y - c->position.y) / scaler.y);
    return world;
}

Vec2 screen_to_world_vec(Camera* c, Vec2 screen) {
    Vec2 scaler = get_aspect_scaler(c);
    return (Vec2){ screen.x * scaler.x, -screen.y * scaler.y };
}

Vec2 world_to_screen_vec(Camera* c, Vec2 world) {
    Vec2 scaler = get_aspect_scaler(c);
    return (Vec2){ scaler.x / world.x, -scaler.y / world.y };
}

Mat4 get_ortho_mat(Camera* c) {
    Vec2 top_left = screen_to_world(c, (Vec2){0.0f, 0.0f});
    Vec2 bottom_right = screen_to_world(c, (Vec2){1.0f, 1.0f});

    float n = -1.0f;
    float f = 1.0f;

    float l = top_left.x;
    float t = top_left.y;
    float r  = bottom_right.x;
    float b = bottom_right.y;

    return (Mat4){{
            2.0f/(r-l), 0.0f,          0.0f,           -(r+l)/(r-l),
            0.0f,       2.0f/(t-b),    0.0f,           -(t+b)/(t-b),
            0.0f,       0.0f,         -2.0f/(f-n),     -(f+n)/(f-n),
            0.0f,       0.0f,          0.0f,            1.0f,
        }};
}
