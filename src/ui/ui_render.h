#ifndef UI_RENDER_H_
#define UI_RENDER_H_

#include "../deps/microui/microui.h"
#include "camera.h"
#include "platform/platform.h"
#include "state.h"

#ifdef DEBUG
    void ui_render_init(mu_Context* debug_ui_context);
#else
    void ui_render_init(void);
#endif

void ui_render_uninit(void);

void ui_render_frame(PlatformFrameData frame, Camera cam, State* state);

#endif
