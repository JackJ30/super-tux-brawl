#include "mu_implementation.h"
#include "platform/gpu_pipeline.h"
#include <SDL3/SDL_render.h>
#include <stdlib.h>

/*

if (frame.swapchain_texture != NULL) {
    SDL_GPUCommandBuffer* cmd = frame.cmd;

    SDL_GPUColorTargetInfo color_target = {
        .texture    = frame.swapchain_texture,
        .load_op    = SDL_GPU_LOADOP_CLEAR,
        .store_op   = SDL_GPU_STOREOP_STORE,
        .clear_color = (SDL_FColor){ 0.0f, 0.4f, 0.6f, 1.0f },
    };

    SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd, &color_target, 1, NULL);
    if (!render_pass) return;

    mat4 view = get_ortho_mat(&cam);

    SDL_BindGPUGraphicsPipeline(render_pass, renderer.pipeline);
    SDL_PushGPUVertexUniformData(cmd, 0, &view, sizeof(view));

    for (int i = 0; i < batch.count; i++) {
        Rect* rect = &batch.rects[i];

        // pack x,y,w,h into a vec4 for the vertex shader
        float xywh[4] = { rect->x, rect->y, rect->w, rect->h };
        float color[4] = { rect->r, rect->g, rect->b, rect->a };

        SDL_PushGPUVertexUniformData(cmd, 1, xywh,  sizeof(xywh));
        SDL_PushGPUFragmentUniformData(cmd, 0, color, sizeof(color));
        SDL_DrawGPUPrimitives(render_pass, 6, 1, 0, 0);
    }

    SDL_EndGPURenderPass(render_pass);

    // clear for next frame
    batch_clear();
}
 */

void r_init(void) {
    batch.rects = NULL;
    batch.count = 0;
    batch.capacity = 1;
}

void r_uninit(void){
    if (batch.rects != NULL)
        free(batch.rects);
    batch.count = 0;
    batch.capacity = 0;
}

void r_clear_batch(void) {
    batch.count = 0; // goes back to start and replaces rects with new info
}

void r_draw_rect(mu_Rect rect, mu_Color color) {
    // resize array for new elements
    if (batch.capacity == 0) {
        batch.capacity = 1;
        batch.rects = malloc(sizeof(MURect));
    }
    if (batch.count >= batch.capacity) {
        batch.capacity *= 1;
        batch.rects = realloc(batch.rects, sizeof(MURect) * batch.capacity);
    }

    // adds rect to current open spot
    batch.rects[batch.count++] = (MURect) {
        .x = rect.x,
        .y = rect.y,
        .w = rect.w,
        .h = rect.h,
        .r = color.r,
        .g = color.g,
        .b = color.b
    };
}

void r_draw_text(const char *text, mu_Vec2 pos, mu_Color color) {

}
void r_draw_icon(int id, mu_Rect rect, mu_Color color) {

}
int r_get_text_width(const char *text, int len) {


    return 0;
}
int r_get_text_height(void) {

    return 0;
}
void r_set_clip_rect(mu_Rect rect) {

}
void r_clear(mu_Color color) {

}
void r_present(void) {

}


