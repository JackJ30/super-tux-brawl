#include "ui_render.h"
#include "../platform/gpu_pipeline.h"
#include "debug/mu_implementation.h"

struct {
    SDL_GPUGraphicsPipeline* debug_ui_pipeline; // this pipeline just has the shaders for debug_ui
} ui_renderer = {0};

void ui_render_init(void) {
#ifdef DEBUG
    ui_renderer.debug_ui_pipeline = graphics_pipeline_load("custom_quad.vert.glsl", "colored.frag.glsl", false);
#else
    ui_render.debug_ui_pipeline = NULL;
#endif
}

void ui_render_frame(PlatformFrameData frame, Camera cam, State* state) {
    // starts loading everything for gpu
    if (frame.swapchain_texture != NULL) {
        // gpu spec init
        SDL_GPUCommandBuffer* cmd = frame.cmd;

        SDL_GPUColorTargetInfo color_target = {
            .texture    = frame.swapchain_texture,
            .load_op    = SDL_GPU_LOADOP_LOAD, // ensures it doesn't clear pass 1
            .store_op   = SDL_GPU_STOREOP_STORE,
            .clear_color = (SDL_FColor){ 0.0f, 0.4f, 0.6f, 1.0f },
        };

        SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd, &color_target, 1, NULL);
        if (!render_pass) return;

        // world camera for shaders
        mat4 view = get_ortho_mat(&cam);

#ifdef DEBUG
        r_load_frame();

        // debug ui render pass
        SDL_BindGPUGraphicsPipeline(render_pass, ui_renderer.debug_ui_pipeline);
        SDL_PushGPUVertexUniformData(cmd, 0, &view, sizeof(view));

        for (int i = 0; i < batch.count; i++) {
            MURect* rect = &batch.rects[i];

            // packs info into vecs for shaders
            float xywh[4] = { rect->x, rect->y, rect->w, rect->h };
            float color[3] = { rect->r, rect->g, rect->b };

            // pushes everything to shaders to be rendered
            SDL_PushGPUVertexUniformData(cmd, 1, xywh,  sizeof(xywh));
            SDL_PushGPUFragmentUniformData(cmd, 0, color, sizeof(color));
            SDL_DrawGPUPrimitives(render_pass, 6, 1, 0, 0);
        }

        // clears current debug ui frame for next run
        r_clear_batch();
#endif

        SDL_EndGPURenderPass(render_pass);
    }

}

