#include <SDL3/SDL.h>

#include "logger.h"

int width = 600, height = 800;

int main() {
	for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i) {
		if (strcmp(SDL_GetVideoDriver(i), "wayland") == 0) {
			SDL_SetHintWithPriority(SDL_HINT_VIDEO_DRIVER, "wayland", SDL_HINT_OVERRIDE);
		}
	}

    // init sdl
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		log_err("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
	}

    // create sdl window
    SDL_Window* window = SDL_CreateWindow("Super Tux Brawl", width, height, SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
		log_err("Failed to create SDL window: %s", SDL_GetError());
        return 1;
    }

    // set up gpu
    SDL_GPUDevice* gpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_MSL, true, NULL);
    if (gpu == NULL) {
		log_err("Failed to create SDL GPU: %s", SDL_GetError());
        return 1;
    }
    if (!SDL_SetGPUAllowedFramesInFlight(gpu, 2)) {
		log_err("Failed to set frames in flight: %s", SDL_GetError());
        return 1;
    }
    if (!SDL_ClaimWindowForGPUDevice(gpu, window)) {
		log_err("Failed to connect gpu to window: %s", SDL_GetError());
        return 1;
    }

    // set up swapchain
    // todo(jack): vsync setting
    if (!SDL_SetGPUSwapchainParameters(gpu, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC)) {
		log_err("Failed to set swapchain parameters: %s", SDL_GetError());
        return 1;
    }

    bool running = true;
    while (running) {

        // poll events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_EVENT_QUIT: {
                    running = false;
                    continue;
                }
            }
        }

        // === render ===
        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(gpu);
        SDL_GPUTexture* swapchain_texture;
        SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window, &swapchain_texture, NULL, NULL);
        if (swapchain_texture != NULL) {
            // render pass
            SDL_GPUColorTargetInfo color_target = {
                .texture = swapchain_texture,
                .load_op = SDL_GPU_LOADOP_CLEAR,
                .store_op = SDL_GPU_STOREOP_STORE,
                .clear_color = (SDL_FColor){ .r = 0.0f, .g = 0.4f, .b = 0.6, .a=1.0f },
            };
            SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd, &color_target, 1, NULL);
            if (!render_pass) continue;

            // render


            // end render pass
            SDL_EndGPURenderPass(render_pass);
        }

        if (!SDL_SubmitGPUCommandBuffer(cmd)) {
            log_err("Failed to submit gpu command buffer: %s", SDL_GetError());
        }
    }

    // cleanup
    SDL_DestroyGPUDevice(gpu);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
