#include <SDL3/SDL.h>

#include "camera.h"
#include "platform.h"
#include "inc.h"
#include "renderer.h"
#include "network.h"

int main() {

    init_tmp();

    if (platform_init() != 0) {
        return 1;
    }

    if (renderer_init(platform.window) != 0) {
        return 1;
    }

    // main loop
    Camera camera = { .scale = 3.0f, .aspect=1.0f };
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
                 // case SDL_EVENT_KEY_DOWN: {
                 //     switch (e.key.key) {
                 //         case SDLK_O: // opens server
                 //             network_server_create(NETWORK_SERVER_PORT, NETWORK_MAX_CLIENTS);
                 //             break;
                 //         case SDLK_D: // destroy server
                 //             network_server_destroy();
                 //         case SDLK_C: // connects server
                 //             network_client_connect("localhost", NETWORK_SERVER_PORT);
                 //             break;
                 //         case SDLK_W: // send basic packet
                 //             network_client_send_packet("hello");
                 //             break;
                 //         case SDLK_P:
                 //             network_client_disconnect();
                 //             break;
                 //         case SDLK_E:
                 //             network_client_poll();
                 //             break;
                 //     }
                 // }
                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
                    camera.aspect = (float)e.window.data1 / (float)e.window.data2;
                }
            }
        }

        render_frame(platform.window, &camera);

        // clear arena
        reset_tmp();
    }

    // cleanup
    renderer_shutdown();
    platform_shutdown();
    shutdown_tmp();

    return 0;
}
