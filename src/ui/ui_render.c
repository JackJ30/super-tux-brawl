#include "ui_render.h"
#include "../platform/gpu_pipeline.h"
#include "debug/debug_ui.h"
#include "debug/mu_implementation.h"

#ifdef DEBUG
    mu_Context* debug_ui_context;
#endif

struct {
    SDL_GPUGraphicsPipeline* pipeline;
} ui_renderer = {0};

void ui_render_init(mu_Context* debug_ui_context) {
    debug_ui_context = NULL;
    ui_renderer.pipeline = graphics_pipeline_load("custom_quad.vert.glsl", "colored.frag.glsl", false);
}
void ui_render_uninit(void) {
    graphics_pipeline_destroy(ui_renderer.pipeline);
}

void ui_render_frame(PlatformFrameData frame, Camera cam, State* state) {
#ifdef DEBUG
    // debug ui
    debug_ui_process_frame(debug_ui_context);
    r_clear(mu_color(90, 95, 100, 255));
    mu_Command* cmd = NULL;
    while (mu_next_command(debug_ui_context, &cmd)) {
        switch (cmd->type) {
            case MU_COMMAND_TEXT: r_draw_text(cmd->text.str, cmd->text.pos, cmd->text.color); break;
            case MU_COMMAND_RECT: r_draw_rect(cmd->rect.rect, cmd->rect.color); break;
            case MU_COMMAND_ICON: r_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color); break;
            case MU_COMMAND_CLIP: r_set_clip_rect(cmd->clip.rect); break;
        }
    }

    // now goes to render everything
    if (frame.swapchain_texture != NULL) {
        SDL_GPUCommandBuffer* cmd = frame.cmd;

        SDL_GPUColorTargetInfo color_target = {
            .texture    = frame.swapchain_texture,
            .load_op    = SDL_GPU_LOADOP_LOAD, // ensures it doesn't clear pass 1
            .store_op   = SDL_GPU_STOREOP_STORE,
            .clear_color = (SDL_FColor){ 0.0f, 0.4f, 0.6f, 1.0f },
        };

        SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd, &color_target, 1, NULL);
        if (!render_pass) return;

        mat4 view = get_ortho_mat(&cam);

        SDL_BindGPUGraphicsPipeline(render_pass, ui_renderer.pipeline);
        SDL_PushGPUVertexUniformData(cmd, 0, &view, sizeof(view));

        for (int i = 0; i < batch.count; i++) {
            MURect* rect = &batch.rects[i];

            // packs info into vecs for shaders
            float xywh[4] = { rect->x, rect->y, rect->w, rect->h };
            float color[3] = { rect->r, rect->g, rect->b };

            SDL_PushGPUVertexUniformData(cmd, 1, xywh,  sizeof(xywh));
            SDL_PushGPUFragmentUniformData(cmd, 0, color, sizeof(color));
            SDL_DrawGPUPrimitives(render_pass, 6, 1, 0, 0);
        }

        SDL_EndGPURenderPass(render_pass);

        // clear for next frame
        r_clear_batch();
    }
#endif

}

