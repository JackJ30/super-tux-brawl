#include <SDL3/SDL.h>

#include "arena.h"
#include "logger.h"
#include "platform.h"
#include "inc.h"
#include "renderer.h"

int main() {

    init_tmp();

    char* str = tprintf("Test");
    log_info("%s", str);

    if (platform_init() != 0) {
        return 1;
    }

    if (renderer_init(platform.window) != 0) {
        return 1;
    }

    // main loop
    b8 running = true;
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

        // clear arena
        reset_tmp();
    }

    // cleanup
    renderer_shutdown();
    platform_shutdown();
    shutdown_tmp();

    return 0;
}
