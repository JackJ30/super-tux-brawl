#include "ui.h"
#include "util/arena.h"

void ui_init(void) {
#ifdef DEBUG
    r_init(); // init abstraction
    ArenaMark s = get_scratch_arena(NULL, 0);
    debug_ui_context = (mu_Context*)arena_alloc(s.arena, sizeof(mu_Context), 1);
    mu_init(debug_ui_context); // init microui librariy
    // debug_ui_contex->text_width = text_width;
    // debug_ui_contex->text_height = text_height;
#endif
}

void ui_uninit(void) {

}

void ui_event(SDL_Event* e) {
#ifdef DEBUG
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
