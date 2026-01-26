#ifndef RENDERER_H_
#define RENDERER_H_

#include "camera.h"
#include "world.h"

#include <SDL3/SDL_gpu.h>

typedef struct {
    SDL_GPUDevice* gpu;
    SDL_GPUGraphicsPipeline* pipeline;
} RendererState;

extern RendererState renderer;

int renderer_init(SDL_Window* window);
void renderer_shutdown();

void render_frame(SDL_Window* window, Camera* cam, State* state);

#endif
