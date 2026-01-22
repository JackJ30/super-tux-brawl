#include "renderer.h"
#include "logger.h"

RendererState renderer;

int rendererInit(SDL_Window* window) {

    // set up gpu
    renderer.gpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_MSL, DEBUG, NULL);
    if (renderer.gpu == NULL) {
		log_err("Failed to create SDL GPU: %s", SDL_GetError());
        return 1;
    }
    if (!SDL_SetGPUAllowedFramesInFlight(renderer.gpu, 2)) {
		log_err("Failed to set frames in flight: %s", SDL_GetError());
        return 1;
    }
    if (!SDL_ClaimWindowForGPUDevice(renderer.gpu, window)) {
		log_err("Failed to connect gpu to window: %s", SDL_GetError());
        return 1;
    }

    // set up swapchain
    // todo(jack): vsync setting
    if (!SDL_SetGPUSwapchainParameters(renderer.gpu, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC)) {
		log_err("Failed to set swapchain parameters: %s", SDL_GetError());
        return 1;
    }

    return 0;
}

void rendererShutdown() {
    SDL_DestroyGPUDevice(renderer.gpu);
}

void renderFrame(SDL_Window* window) {

    // get render resources
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(renderer.gpu);
    if (!cmd) {
        log_err("Failed to acquire command buffer: %s", SDL_GetError());
    }
    SDL_GPUTexture* swapchain_texture;
    SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window, &swapchain_texture, NULL, NULL);
    if (!swapchain_texture) {
        log_err("Failed to acquire swapchain texture: %s", SDL_GetError());
    }

    // render if we can
    if (swapchain_texture != NULL) {

        // render pass
        SDL_GPUColorTargetInfo color_target = {
            .texture = swapchain_texture,
            .load_op = SDL_GPU_LOADOP_CLEAR,
            .store_op = SDL_GPU_STOREOP_STORE,
            .clear_color = (SDL_FColor){ .r = 0.0f, .g = 0.4f, .b = 0.6, .a=1.0f },
        };
        SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd, &color_target, 1, NULL);
        if (!render_pass) return;

        // render

        // end render pass
        SDL_EndGPURenderPass(render_pass);
    }

    // submit command buffer
    if (!SDL_SubmitGPUCommandBuffer(cmd)) {
        log_err("Failed to submit gpu command buffer: %s", SDL_GetError());
    }
}
