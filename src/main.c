#include <SDL3/SDL.h>

#include "camera.h"
#include "input.h"
#include "net.h"
#include "platform.h"
#include "inc.h"
#include "renderer.h"
#include "logger.h"

int main(int argc, char** argv) {

    /* read args */

    bool server = false;
    for (size i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--server") == 0) {
            server = true;
        }
    }

    /* initialize */

    init_tmp();

    if (platform_init() != 0) {
        return 1;
    }

    if (net_init(server) != 0) {
        return 1;
    }

    if (renderer_init(platform.window) != 0) {
        return 1;
    }

    world_init();

    /* main loop */
    Camera camera = { .scale = 3.0f, .aspect=((float)platform.width / (float)platform.height) };
    b8 running = true;
	u64 prev_time = SDL_GetTicksNS();
    while (running) {

        // wait for frame availability
        wait_for_frame(platform.window);

		// get timestamp
		u64 time = SDL_GetTicksNS();
		float dt = (time - prev_time) * 1e-9f;
		prev_time = time;

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

        // scan input
        Input input = {0};
        const bool* key_state = SDL_GetKeyboardState(NULL);
        if (key_state[SDL_SCANCODE_LEFT] || key_state[SDL_SCANCODE_A]) input.direction.x -= 1.0f;
        if (key_state[SDL_SCANCODE_RIGHT] || key_state[SDL_SCANCODE_D]) input.direction.x += 1.0f;
        if (key_state[SDL_SCANCODE_C] || key_state[SDL_SCANCODE_SPACE]) input.jump = true;

        // simulate
        State* new_state = world_sim(dt, &input);

        // render
        render_frame(&camera, new_state);

        // clear arena
        reset_tmp();
    }

    // cleanup
    world_shutdown();
    renderer_shutdown();
    net_shutdown();
    platform_shutdown();
    shutdown_tmp();

    return 0;
}
