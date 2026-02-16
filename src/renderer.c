#include "renderer.h"

#include "platform/gpu_pipeline.h"
#include "util/da.h"

struct {
    SDL_GPUGraphicsPipeline* pipeline;
} renderer = {0};

int renderer_init(SDL_Window* window) {

    /* initialize pipelines */
    renderer.pipeline = graphics_pipeline_load("quad.vert", "colored.frag", false);

    return 0;
}

void renderer_shutdown() {
    graphics_pipeline_destroy(renderer.pipeline);
}

void render_frame(PlatformFrameData frame, Camera* cam, State* state) {

    // render if we can
    if (frame.swapchain_texture != NULL) {

        SDL_GPUCommandBuffer* cmd = frame.cmd;

        // render pass
        SDL_GPUColorTargetInfo color_target = {
            .texture = frame.swapchain_texture,
            .load_op = SDL_GPU_LOADOP_CLEAR,
            .store_op = SDL_GPU_STOREOP_STORE,
            .clear_color = (SDL_FColor){ .r = 0.0f, .g = 0.4f, .b = 0.6, .a=1.0f },
        };
        SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd, &color_target, 1, NULL);
        if (!render_pass) return;

        // constants
        mat4 view = get_ortho_mat(cam);

        // render guys
        SDL_BindGPUGraphicsPipeline(render_pass, renderer.pipeline);
        SDL_PushGPUVertexUniformData(cmd, 0, &view, sizeof(view));
        for (size i = 0; i < da_size(state->guys); ++i) {
            Guy* g = &state->guys[i];

            SDL_PushGPUVertexUniformData(cmd, 1, &g->position, sizeof(vec2));
            vec3 color = i == state->owned_guy ? (vec3){0.0f, 0.2f, 0.5f} : (vec3){0.5f, 0.2f, 0.2f};
            SDL_PushGPUFragmentUniformData(cmd, 0, &color, sizeof(vec3));
            SDL_DrawGPUPrimitives(render_pass, 6, 1, 0, 0);
        }

        // end render pass
        SDL_EndGPURenderPass(render_pass);
    }
}
