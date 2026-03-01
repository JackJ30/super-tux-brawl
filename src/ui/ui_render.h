#ifndef UI_RENDER_H_
#define UI_RENDER_H_

#include "camera.h"
#include "platform/platform.h"
#include "state.h"

void ui_render_init(void);
void ui_render_uninit(void);

void ui_render_frame(PlatformFrameData frame, Camera cam, State* state);

#endif
