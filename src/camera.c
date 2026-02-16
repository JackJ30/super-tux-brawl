#include "camera.h"

vec2 get_aspect_scaler(Camera* c) {
    if (c->aspect >= 1.0f) {
        return (vec2){ c->scale * c->aspect, c->scale };
    } else {
        return (vec2){ c->scale, c->scale * (1.0f / c->aspect)};
    }
}

vec2 screen_to_world(Camera* c, vec2 screen) {
    vec2 scaler = get_aspect_scaler(c);
    screen.x = c->position.x + ((screen.x - 0.5f) * scaler.x);
    screen.y = c->position.y + ((0.5f - screen.y) * scaler.y);
    return screen;
}

vec2 world_to_screen(Camera* c, vec2 world) {
    vec2 scaler = get_aspect_scaler(c);
    world.x = ((world.x - c->position.x) / scaler.x) + 0.5f;
    world.y = 0.5f - ((world.y - c->position.y) / scaler.y);
    return world;
}

vec2 screen_to_world_vec(Camera* c, vec2 screen) {
    vec2 scaler = get_aspect_scaler(c);
    return (vec2){ screen.x * scaler.x, -screen.y * scaler.y };
}

vec2 world_to_screen_vec(Camera* c, vec2 world) {
    vec2 scaler = get_aspect_scaler(c);
    return (vec2){ scaler.x / world.x, -scaler.y / world.y };
}

mat4 get_ortho_mat(Camera* c) {
    vec2 top_left = screen_to_world(c, (vec2){0.0f, 0.0f});
    vec2 bottom_right = screen_to_world(c, (vec2){1.0f, 1.0f});

    float n = -1.0f;
    float f = 1.0f;

    float l = top_left.x;
    float t = top_left.y;
    float r  = bottom_right.x;
    float b = bottom_right.y;

    return (mat4){{
            2.0f/(r-l), 0.0f,          0.0f,           -(r+l)/(r-l),
            0.0f,       2.0f/(t-b),    0.0f,           -(t+b)/(t-b),
            0.0f,       0.0f,         -2.0f/(f-n),     -(f+n)/(f-n),
            0.0f,       0.0f,          0.0f,            1.0f,
        }};
}
