#include "util/arena.h"
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

#if DEBUG
#include "ui/debug/debug_ui.h"
mu_Context* debug_ui_context = NULL;
#endif

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

    // ui
#if DEBUG
    r_init(); // init abstraction
    ArenaMark s = get_scratch_arena(NULL, 0);
    debug_ui_context = (mu_Context*)arena_alloc(s.arena, sizeof(mu_Context), 1);
    mu_init(debug_ui_context); // init microui librariy
    // debug_ui_contex->text_width = text_width;
    // debug_ui_contex->text_height = text_height;
#endif


	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {

    if (!headless) {
        /* process and render client */
        client_process();

#if DEBUG
        // debug ui
        debug_ui_process_frame(debug_ui_context);
        r_clear(mu_color(90, 95, 100, 255));
        mu_Command* cmd = NULL;
        while (mu_next_command(debug_ui_context, &cmd)) {
            switch (cmd->type) {
                case MU_COMMAND_TEXT: r_draw_text(cmd->text.str, cmd->text.pos, cmd->text.color); break;
                case MU_COMMAND_RECT: r_draw_rect(cmd->rect.rect, cmd->rect.color); break;
                case MU_COMMAND_ICON: r_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color); break;
                case MU_COMMAND_CLIP: r_set_clip_rect(cmd->clip.rect); break;
            }
        }
        r_present();
#endif
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

#if DEBUG
        // debug ui

        switch (e->type) {
            case SDL_EVENT_MOUSE_MOTION: mu_input_mousemove(debug_ui_context, e->motion.x, e->motion.y); break;
            case SDL_EVENT_MOUSE_WHEEL: mu_input_scroll(debug_ui_context, 0, e->wheel.y * -30); break;
            case SDL_EVENT_TEXT_INPUT: mu_input_text(debug_ui_context, e->text.text); break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                int b = button_map[e->button.button & 0xff];
                if (b && e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) { mu_input_mousedown(debug_ui_context, e->button.x, e->button.y, b); }
                if (b && e->type ==   SDL_EVENT_MOUSE_BUTTON_UP) { mu_input_mouseup(debug_ui_context, e->button.x, e->button.y, b);   }
                break;
            }

            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP: {
                int c = key_map[e->key.down & 0xff];
                if (c && e->type == SDL_EVENT_KEY_DOWN) { mu_input_keydown(debug_ui_context, c); }
                if (c && e->type ==   SDL_EVENT_KEY_UP) { mu_input_keyup(debug_ui_context, c);   }
                break;
            }
        }
#endif
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
