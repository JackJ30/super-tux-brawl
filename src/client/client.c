#include "client.h"

#include "client_net.h"

#include "state.h"
#include "camera.h"
#include "input.h"
#include "renderer.h"
#include "server/server.h"

Input input = {0};
State state = {0};
Camera camera = {0};

u64 prev_time = 0;
b8 running_server = false;

int client_init(b8 self_host) {

    /* start server if self host */
    if (self_host) {
        if (server_start(0) != 0) {
            return 1;
        }
        running_server = true;
    }

    /* net */
    if (client_net_init("localhost", 0) != 0) {
        return 1;
    }

    /* renderer */
    if (renderer_init() != 0) {
        return 1;
    }

    /* state */
    state.guys = da_create(Guy, 0);
    create_guy(&state);

    camera = (Camera){ .scale = 3.0f, .aspect=((float)platform.width / (float)platform.height) };
    prev_time = SDL_GetTicksNS();

    return 0;
}

void client_shutdown() {
    da_destroy(state.guys);
    renderer_shutdown();
    client_net_shutdown();

    if (running_server) {
        server_finish();
    }
}

void client_event(SDL_Event* e) {
    switch (e->type) {
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
            platform.width = e->window.data1;
            platform.height = e->window.data2;
            camera.aspect = (float)platform.width / (float)platform.height;
            break;
        }
        case SDL_EVENT_KEY_DOWN: {
            if (e->key.key == SDLK_Z) {
                input.swap = true;
            }
        }
    }
}

void client_process() {

    // wait for frame availability
    PlatformFrameData frame = platform_wait_for_frame();

    // get timestamp
    u64 time = SDL_GetTicksNS();
    float dt = (time - prev_time) * 1e-9f;
    prev_time = time;

    // process networking
    client_net_process();

    // scan input
    const bool* key_state = SDL_GetKeyboardState(NULL);
    if (key_state[SDL_SCANCODE_LEFT] || key_state[SDL_SCANCODE_A]) input.direction.x -= 1.0f;
    if (key_state[SDL_SCANCODE_RIGHT] || key_state[SDL_SCANCODE_D]) input.direction.x += 1.0f;
    if (key_state[SDL_SCANCODE_C] || key_state[SDL_SCANCODE_SPACE]) input.jump = true;

    // simulate
    sim_tick(&state, dt);

    // render and present
    render_frame(frame, camera, &state);
    platform_submit_frame(frame);

    // clear arena
    reset_tmp();

    // reset input
    input = (Input){0};
}
