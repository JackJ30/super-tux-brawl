#include "ui_render.h"

// struct {
//     SDL_GPUGraphicsPipeline* pipeline;
// } renderer = {0};
//
// int renderer_init() {
//
//     /* initialize pipelines */
//     renderer.pipeline = graphics_pipeline_load("quad.vert", "colored.frag", false);
//
//     return 0;
// }
//
// void renderer_shutdown() {
//     graphics_pipeline_destroy(renderer.pipeline);
// }
//
// void render_frame(PlatformFrameData frame, Camera cam, State* state) {
//
//     // render if we can
//     if (frame.swapchain_texture != NULL) {
//
//         SDL_GPUCommandBuffer* cmd = frame.cmd;
//
//         // render pass
//         SDL_GPUColorTargetInfo color_target = { .texture = frame.swapchain_texture,
//             .load_op = SDL_GPU_LOADOP_CLEAR,
//             .store_op = SDL_GPU_STOREOP_STORE,
//             .clear_color = (SDL_FColor){ .r = 0.0f, .g = 0.4f, .b = 0.6, .a=1.0f },
//         };
//         SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd, &color_target, 1, NULL);
//         if (!render_pass) return;
//
//         // constants
//         mat4 view = get_ortho_mat(&cam);
//
//         // render guys
//         SDL_BindGPUGraphicsPipeline(render_pass, renderer.pipeline);
//         SDL_PushGPUVertexUniformData(cmd, 0, &view, sizeof(view));
//         for (size i = 0; i < da_size(state->guys); ++i) {
//             Guy* g = &state->guys[i];
//
//             SDL_PushGPUVertexUniformData(cmd, 1, &g->position, sizeof(vec2));
//             SDL_PushGPUFragmentUniformData(cmd, 0, &(vec3){0.0f, 0.2f, 0.5f}, sizeof(vec3));
//             SDL_DrawGPUPrimitives(render_pass, 6, 1, 0, 0);
//         }
//
//         // end render pass
//         SDL_EndGPURenderPass(render_pass);
//     }
// }

void ui_render_init(void) {


}
void ui_render_uninit(void) {

}


void ui_render_frame(mu_Context* debug_ui_context) {
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
    r_present();
#endif

}

