/* This file is responsible for setting up the app, launching services and
running the client. Detailed explanation below. */

#include "util/inc.h"
#include "platform/platform.h"
#include "util/logger.h"

#include "net/net.h"
#include "server.h"
#include "client.h"

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

bool is_server = true;
bool is_client = true;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {

    /* read args */
    for (size i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--headless") == 0) {
            is_client = false;
            is_server = true;
        }
        if (strcmp(argv[i], "--connect") == 0) {
            is_client = true;
            is_server = false;
        }
    }

    /* initialize */

    // temporary memory
    init_tmp();

    // platform
    if (platform_init(is_client) != 0) {
        return SDL_APP_FAILURE;
    }

    /* setup systems and services */

    // start net
    net_start(is_server, is_client);

    // start server
    if (is_server) {
        log_info("Starting server.");
        server_start();
    }

    // initialize client
    if (is_client) {
        log_info("Starting client.");
        client_init();
    }

    if (!is_server && !is_client) {
        log_warn("You probably didn't mean to run without client or server.");
        return SDL_APP_SUCCESS;
    }

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {

    if (is_client) {

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

    /* send event to client */
    client_event(e);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {

    // cleanup client
    if (is_client) client_shutdown();

    // join server
    if (is_server) server_finish();

    // join net
    net_finish();

    /* shutdown */
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
