#ifndef GPU_PIPELINE_H_
#define GPU_PIPELINE_H_

#include <SDL3/SDL_gpu.h>

#include "inc.h"

SDL_GPUShaderFormat get_availale_shader_formats();
SDL_GPUGraphicsPipeline* graphics_pipeline_load(char* vertex_path, char* fragment_path, b32 blend);
void graphics_pipeline_destroy(SDL_GPUGraphicsPipeline*);

#endif
