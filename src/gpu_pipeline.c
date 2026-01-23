#include "gpu_pipeline.h"

#include "logger.h"
#include "platform.h"
#include "renderer.h"

SDL_GPUShader* load_shader(char* path) {
    // todo(jack): implement
    assert(false);
}

SDL_GPUShaderFormat get_availale_shader_formats() {
    // todo(jack&riley): make this use a manifest or define
    return SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_MSL;
}

SDL_GPUGraphicsPipeline* graphics_pipeline_load(char* vertex_path, char* fragment_path, b8 blend) {

    // load shaders
    SDL_GPUShader* vertex = load_shader(vertex_path);
    SDL_GPUShader* fragment = load_shader(fragment_path);

    // create pipeline
    SDL_GPUGraphicsPipelineCreateInfo info = {
        .target_info = (SDL_GPUGraphicsPipelineTargetInfo){
            .num_color_targets = 1,
            .color_target_descriptions = &(SDL_GPUColorTargetDescription){
                .format = SDL_GetGPUSwapchainTextureFormat(renderer.gpu, platform.window)
            },
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .rasterizer_state = (SDL_GPURasterizerState){
            .fill_mode = SDL_GPU_FILLMODE_FILL,
        },
        .vertex_shader = vertex,
        .fragment_shader = fragment,
    };
    if (blend) {
        ((SDL_GPUColorTargetDescription*)info.target_info.color_target_descriptions)[0].blend_state = (SDL_GPUColorTargetBlendState){
            .enable_blend = true,
            .color_blend_op = SDL_GPU_BLENDOP_ADD,
            .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
            .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
            .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
            .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        };
    }

    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(renderer.gpu, &info);
    if (pipeline == NULL) {
        log_err("Failed to create graphics pipeline: %s", SDL_GetError());
        return NULL;
    }

    SDL_ReleaseGPUShader(renderer.gpu, vertex);
    SDL_ReleaseGPUShader(renderer.gpu, fragment);
    return pipeline;
}

void graphics_pipeline_destroy(SDL_GPUGraphicsPipeline* pipeline) {
    SDL_ReleaseGPUGraphicsPipeline(renderer.gpu, pipeline);
}
