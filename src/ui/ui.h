#ifndef UI_H_
#define UI_H_

#include "ui_render.h"
#include <SDL3/SDL.h>

#ifdef DEBUG
#include "debug/debug_ui.h"
mu_Context* debug_ui_context = NULL; // pass this into ui_render.h ui_render_frame(...)
#endif


void ui_init(void);
void ui_uninit(void);

void ui_event(SDL_Event* event);


#endif
