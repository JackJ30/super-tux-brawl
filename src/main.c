#include <SDL3/SDL.h>

#include "platform.h"
#include "renderer.h"

int main() {

    if (platform_init() != 0) {
        return 1;
    }

    if (renderer_init(platform.window) != 0) {
        return 1;
    }

    // main loop
    bool running = true;
    while (running) {
        // poll events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_EVENT_QUIT: {
                    running = false;
                    continue;
                }
            }
        }

        render_frame(platform.window);
    }

    // cleanup
    renderer_shutdown();
    platform_shutdown();

    return 0;
}
