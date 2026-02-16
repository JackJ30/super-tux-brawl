#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <SDL3/SDL.h>

typedef struct {
    SDL_Window* window;
    SDL_GPUDevice* gpu;
    int width, height;
} PlatformState;

extern PlatformState platform;

int platform_init();
void platform_shutdown();

typedef struct {
    SDL_GPUCommandBuffer* cmd;
    SDL_GPUTexture* swapchain_texture;
} PlatformFrameData;

PlatformFrameData platform_wait_for_frame();
void platform_submit_frame(PlatformFrameData frame);

#endif
