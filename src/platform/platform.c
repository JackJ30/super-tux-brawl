#include "platform.h"

#include "util/logger.h"
#include "util/config.h"

#include "gpu_pipeline.h"

PlatformState platform = {};

int platform_init() {
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
    platform.width = 800;
    platform.height = 600;
    platform.window = SDL_CreateWindow("Super Tux Brawl", platform.width, platform.height, SDL_WINDOW_RESIZABLE);
    if (platform.window == NULL) {
		log_err("Failed to create SDL window: %s", SDL_GetError());
        return 1;
    }

    /* set up gpu */

    platform.gpu = SDL_CreateGPUDevice(get_availale_shader_formats(), DEBUG, NULL);
    if (platform.gpu == NULL) {
		log_err("Failed to create SDL GPU: %s", SDL_GetError());
        return 1;
    }
    if (!SDL_SetGPUAllowedFramesInFlight(platform.gpu, 2)) {
		log_err("Failed to set frames in flight: %s", SDL_GetError());
        return 1;
    }
    if (!SDL_ClaimWindowForGPUDevice(platform.gpu, platform.window)) {
		log_err("Failed to connect gpu to window: %s", SDL_GetError());
        return 1;
    }

    /* set up swapchain */

    // find best present mode
    SDL_GPUPresentMode present_mode = SDL_GPU_PRESENTMODE_VSYNC;
    if (config.vsync) {
        // try mailbox if want vsync
        if (SDL_WindowSupportsGPUPresentMode(platform.gpu, platform.window, SDL_GPU_PRESENTMODE_MAILBOX))
            present_mode = SDL_GPU_PRESENTMODE_MAILBOX;
    } else {
        // try immediate if you don't want vsync
        if (SDL_WindowSupportsGPUPresentMode(platform.gpu, platform.window, SDL_GPU_PRESENTMODE_IMMEDIATE))
            present_mode = SDL_GPU_PRESENTMODE_IMMEDIATE;
    }

    if (!SDL_SetGPUSwapchainParameters(platform.gpu, platform.window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, present_mode)) {
		log_err("Failed to set swapchain parameters: %s", SDL_GetError());
        return 1;
    }

    return 0;
}

void platform_shutdown() {
    SDL_DestroyGPUDevice(platform.gpu);
    SDL_DestroyWindow(platform.window);
    SDL_Quit();
}

PlatformFrameData platform_wait_for_frame() {
    PlatformFrameData ret = {0};

    // get command buffer
    ret.cmd = SDL_AcquireGPUCommandBuffer(platform.gpu);
    if (!ret.cmd) {
        log_err("Failed to acquire command buffer: %s", SDL_GetError());
    }

    // get swapchain texture
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(ret.cmd, platform.window, &ret.swapchain_texture, NULL, NULL)) {
        log_err("Failed to acquire swapchain texture: %s", SDL_GetError());
    }

    return ret;
}

void platform_submit_frame(PlatformFrameData frame) {

    // submit command buffer
    if (!SDL_SubmitGPUCommandBuffer(frame.cmd)) {
        log_err("Failed to submit gpu command buffer: %s", SDL_GetError());
    }
}
