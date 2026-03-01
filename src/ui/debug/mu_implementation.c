#include "mu_implementation.h"
#include "util/arena.h"
#include "debug_ui.h"

#include <SDL3/SDL_render.h>
#include <stdlib.h>

mu_Context* debug_ui_context = NULL;

void r_init(void) {
    // init microui context
    ArenaMark s = get_scratch_arena(NULL, 0);
    debug_ui_context = (mu_Context*)arena_alloc(s.arena, sizeof(mu_Context), 1);
    mu_init(debug_ui_context); // init microui librariy
    // debug_ui_contex->text_width = text_width;
    // debug_ui_contex->text_height = text_height;


    // init rect batches
    batch.rects = NULL;
    batch.count = 0;
    batch.capacity = 1;
}

void r_uninit(void){
    if (batch.rects != NULL)
        free(batch.rects);
    batch.count = 0;
    batch.capacity = 0;
}

void r_event(SDL_Event* e) {
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
}

void r_load_frame(void) {
    // processes and executes all calls to microui create objs
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
}

void r_clear_batch(void) {
    batch.count = 0; // goes back to start and replaces rects with new info
}

void r_draw_rect(mu_Rect rect, mu_Color color) {
    // resize array for new elements
    if (batch.capacity == 0) {
        batch.capacity = 1;
        batch.rects = malloc(sizeof(MURect));
    }
    if (batch.count >= batch.capacity) {
        batch.capacity *= 1;
        batch.rects = realloc(batch.rects, sizeof(MURect) * batch.capacity);
    }

    // adds rect to current open spot
    batch.rects[batch.count++] = (MURect) {
        .x = rect.x,
        .y = rect.y,
        .w = rect.w,
        .h = rect.h,
        .r = color.r,
        .g = color.g,
        .b = color.b
    };
}

void r_draw_text(const char *text, mu_Vec2 pos, mu_Color color) {

}
void r_draw_icon(int id, mu_Rect rect, mu_Color color) {

}
int r_get_text_width(const char *text, int len) {


    return 0;
}
int r_get_text_height(void) {

    return 0;
}
void r_set_clip_rect(mu_Rect rect) {

}
void r_clear(mu_Color color) {

}
void r_present(void) {

}


