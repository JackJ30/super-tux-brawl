#include <SDL3/SDL.h>

#include "camera.h"
#include "platform.h"
#include "inc.h"
#include "renderer.h"

int main() {

    init_tmp();

    if (platform_init() != 0) {
        return 1;
    }

    if (renderer_init(platform.window) != 0) {
        return 1;
    }

    world_init();

    // main loop
    Camera camera = { .scale = 3.0f, .aspect=((float)platform.width / (float)platform.height) };
    b8 running = true;
    while (running) {
        // poll event
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_EVENT_QUIT: {
                    running = false;
                    continue;
                }
                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
                    platform.width = e.window.data1;
                    platform.height = e.window.data2;
                    camera.aspect = (float)platform.width / (float)platform.height;
                }
            }
        }

        State* new_state = world_sim(0.01f);
        render_frame(platform.window, &camera, new_state);

        // clear arena
        reset_tmp();
    }

    // cleanup
    world_shutdown();
    renderer_shutdown();
    platform_shutdown();
    shutdown_tmp();

    return 0;
}
