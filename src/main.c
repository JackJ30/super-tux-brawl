#include "util/inc.h"
#include "platform/platform.h"

#include "net.h"
#include "server/server.h"
#include "client/client.h"
#include "util/logger.h"

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

bool headless = false;
bool self_host = false;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {

    /* read args */
    for (size i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--headless") == 0) {
            headless = true;
        }
        else if (strcmp(argv[i], "--host") == 0) {
            self_host = true;
        } else {
            // warn birdbrain
            log_warn("Bad argument: %s", argv[i]);
        }
    }

    /* initialize */

    // temporary memory
    init_tmp();

    // platform
    if (platform_init(!headless) != 0) {
        return SDL_APP_FAILURE;
    }

    // net
    if (net_init() != 0) {
        return SDL_APP_FAILURE;
    }

    if (headless) {
        // start server if headless
        if (server_start(0) != 0) {
            return SDL_APP_FAILURE;
        }
    } else {
        // init client if not headless
        if (client_init(self_host) != 0) {
            return SDL_APP_FAILURE;
        }
    }

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {

    if (!headless) {
        /* process and render client */
        client_process();

    } else {
        /* if no client, sleep so we don't burn cpu */
        /* this loop doesn't matter to pure server */
        SDL_Delay(10); // 10 ms
    }

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *e) {

    /* quit app */
	if (e->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}

    if (!headless) {
        /* send event to client */
        client_event(e);
    }

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {

    if (result == SDL_APP_FAILURE) return;

    if (headless) {
        // finish server if headless
        server_finish();
    } else {
        // shut down client if not headless
        client_shutdown();
    }

    /* shutdown */
    net_shutdown();
    platform_shutdown();
    shutdown_tmp();
}

// The setup: we will use sdl callbacks.

// There will be a net thread whose job is to recieve net communications and add
// them to a "net queue".

// If client only, iterate will be used for rendering, and will consult the net
// thread on state before rendering.

// If server only, iterate will just sleep for a millisecond or so. There will
// be a server thread which consults the net thread then does server stuff.

// If server and client, iterate will be used for the client as usual. The
// server thread will be used as usual. The net thread however, will secretly
// queue the client's events directly onto the server queue.

// All threads who loop will consult a "continue" variable which will be set to
// false if SDL_Quit is received in the events.
