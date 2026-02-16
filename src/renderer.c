#include "renderer.h"

#include "config.h"
#include "da.h"
#include "gpu_pipeline.h"
#include "logger.h"

RendererState renderer = {0};
typedef struct {
    SDL_GPUCommandBuffer* cmd;
    SDL_GPUTexture* swapchain_texture;
} FrameData;
FrameData frame_data = {0};

int renderer_init(SDL_Window* window) {

    // set up gpu
    renderer.gpu = SDL_CreateGPUDevice(get_availale_shader_formats(), DEBUG, NULL);
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

    /* set up swapchain */

    // find best present mode
    SDL_GPUPresentMode present_mode = SDL_GPU_PRESENTMODE_VSYNC;
    if (config.vsync) {
        // try mailbox if want vsync
        if (SDL_WindowSupportsGPUPresentMode(renderer.gpu, window, SDL_GPU_PRESENTMODE_MAILBOX))
            present_mode = SDL_GPU_PRESENTMODE_MAILBOX;
    } else {
        // try immediate if you don't want vsync
        if (SDL_WindowSupportsGPUPresentMode(renderer.gpu, window, SDL_GPU_PRESENTMODE_IMMEDIATE))
            present_mode = SDL_GPU_PRESENTMODE_IMMEDIATE;
    }

    if (!SDL_SetGPUSwapchainParameters(renderer.gpu, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, present_mode)) {
		log_err("Failed to set swapchain parameters: %s", SDL_GetError());
        return 1;
    }

    /* initialize pipelines */
    renderer.pipeline = graphics_pipeline_load("quad.vert", "colored.frag", false);

    return 0;
}

void renderer_shutdown() {
    graphics_pipeline_destroy(renderer.pipeline);
    SDL_DestroyGPUDevice(renderer.gpu);
}

void wait_for_frame(SDL_Window* window) {
    // get command buffer
    frame_data = (FrameData){0};
    frame_data.cmd = SDL_AcquireGPUCommandBuffer(renderer.gpu);
    if (!frame_data.cmd) {
        log_err("Failed to acquire command buffer: %s", SDL_GetError());
    }

    // get swapchain texture
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(frame_data.cmd, window, &frame_data.swapchain_texture, NULL, NULL)) {
        log_err("Failed to acquire swapchain texture: %s", SDL_GetError());
    }
}

void render_frame(Camera* cam, State* state) {

    SDL_GPUCommandBuffer* cmd = frame_data.cmd;

    // render if we can
    if (frame_data.swapchain_texture != NULL) {

        // render pass
        SDL_GPUColorTargetInfo color_target = {
            .texture = frame_data.swapchain_texture,
            .load_op = SDL_GPU_LOADOP_CLEAR,
            .store_op = SDL_GPU_STOREOP_STORE,
            .clear_color = (SDL_FColor){ .r = 0.0f, .g = 0.4f, .b = 0.6, .a=1.0f },
        };
        SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd, &color_target, 1, NULL);
        if (!render_pass) return;

        // constants
        Mat4 view = get_ortho_mat(cam);

        // render guys
        SDL_BindGPUGraphicsPipeline(render_pass, renderer.pipeline);
        SDL_PushGPUVertexUniformData(cmd, 0, &view, sizeof(view));
        for (size i = 0; i < da_size(state->guys); ++i) {
            Guy* g = &state->guys[i];

            SDL_PushGPUVertexUniformData(cmd, 1, &g->position, sizeof(Vec2));
            Vec3 color = i == state->owned_guy ? (Vec3){0.0f, 0.2f, 0.5f} : (Vec3){0.5f, 0.2f, 0.2f};
            SDL_PushGPUFragmentUniformData(cmd, 0, &color, sizeof(Vec3));
            SDL_DrawGPUPrimitives(render_pass, 6, 1, 0, 0);
        }

        // end render pass
        SDL_EndGPURenderPass(render_pass);
    }

    // submit command buffer
    if (!SDL_SubmitGPUCommandBuffer(cmd)) {
        log_err("Failed to submit gpu command buffer: %s", SDL_GetError());
    }
}
