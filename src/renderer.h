#ifndef RENDERER_H_
#define RENDERER_H_

#include "camera.h"
#include "platform/platform.h"
#include "world.h"

#include <SDL3/SDL_gpu.h>

int renderer_init();
void renderer_shutdown();

void render_frame(PlatformFrameData frame, Camera cam, State* state);

#endif
