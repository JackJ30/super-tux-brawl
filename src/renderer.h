#ifndef RENDERER_H_
#define RENDERER_H_

#include <SDL3/SDL_gpu.h>

typedef struct {
    SDL_GPUDevice* gpu;
} RendererState;

extern RendererState renderer;

int rendererInit(SDL_Window* window);
void rendererShutdown();

void renderFrame(SDL_Window* window);

#endif
