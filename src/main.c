#include <SDL3/SDL.h>

#include "platform.h"
#include "renderer.h"

int main() {

    if (platformInit() != 0) {
        return 1;
    }

    if (rendererInit(platform.window) != 0) {
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

        renderFrame(platform.window);
    }

    // cleanup
    rendererShutdown();
    platformShutdown();

    return 0;
}
