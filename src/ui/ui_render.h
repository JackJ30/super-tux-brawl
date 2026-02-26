#ifndef UI_RENDER_H_
#define UI_RENDER_H_

#include "../deps/microui/microui.h"

void ui_render_init(void);
void ui_render_uninit(void);

void ui_render_frame(mu_Context* debug_ui_context);

#endif
