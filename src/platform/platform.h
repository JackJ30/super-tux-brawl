#ifndef PLATFORM_H_
#define PLATFORM_H_

#include "util/inc.h"

#include <SDL3/SDL.h>

typedef struct {
    b8 has_graphics;
    // graphics only
    SDL_Window* window;
    SDL_GPUDevice* gpu;
    i32 width, height;
} PlatformState;

extern PlatformState platform;

/* platform */

int platform_init(b8 graphics);
void platform_shutdown();

/* graphics */

typedef struct {
    SDL_GPUCommandBuffer* cmd;
    SDL_GPUTexture* swapchain_texture;
} PlatformFrameData;

PlatformFrameData platform_wait_for_frame();
void platform_submit_frame(PlatformFrameData frame);

#endif
