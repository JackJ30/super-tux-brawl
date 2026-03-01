#ifndef UI_H_
#define UI_H_

#include "ui_render.h"
#include <SDL3/SDL.h>

void ui_init(void);
void ui_uninit(void);

void ui_event(SDL_Event* event);


#endif
