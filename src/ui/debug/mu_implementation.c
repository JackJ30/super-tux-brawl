#include "mu_implementation.h"
#include "util/arena.h"
#include "debug_ui.h"

#include <SDL3/SDL_render.h>
#include <stdlib.h>

#include "atlas.inl"

static mu_Context* debug_ui_context = NULL;
static SDL_GPUTexture* atlas_gpu_texture = NULL;
static SDL_GPUSampler* atlas_sampler = NULL;

// #include "atlas.inl"
//
// static SDL_Texture* atlas_texture = NULL; // or whatever your renderer uses
//
// void r_init(void) {
//     // ... your existing init ...
//
//     // atlas[] is uint8_t RGBA, 128*128 pixels
//     // create texture from it — exact API depends on your backend
//     // SDL_Renderer example:
//     atlas_texture = SDL_CreateTexture(renderer,
//         SDL_PIXELFORMAT_RGBA32,
//         SDL_TEXTUREACCESS_STATIC,
//         ATLAS_WIDTH, ATLAS_HEIGHT); // 128, 128
//     SDL_UpdateTexture(atlas_texture, NULL, atlas, ATLAS_WIDTH * 4);
//     SDL_SetTextureBlendMode(atlas_texture, SDL_BLENDMODE_BLEND);
// }

void r_init(SDL_GPUDevice* device) {
    // init microui context
    ArenaMark s = get_scratch_arena(NULL, 0);
    debug_ui_context = (mu_Context*)arena_alloc(s.arena, sizeof(mu_Context), 1);
    mu_init(debug_ui_context); // init microui librariy
    // debug_ui_contex->text_width = text_width;
    // debug_ui_contex->text_height = text_height;

    // init atlas, create texture
    if (device != NULL) {
        SDL_GPUTextureCreateInfo tci = {
            .type   = SDL_GPU_TEXTURETYPE_2D,
            .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
            .width  = ATLAS_WIDTH,   // 128
            .height = ATLAS_HEIGHT,  // 128
            .layer_count_or_depth = 1,
            .num_levels = 1,
            .usage  = SDL_GPU_TEXTUREUSAGE_SAMPLER,
        };
        atlas_gpu_texture = SDL_CreateGPUTexture(device, &tci);

        // upload atlas pixel data via transfer buffer
        SDL_GPUTransferBufferCreateInfo tbci = {
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size  = ATLAS_WIDTH * ATLAS_HEIGHT * 4,
        };
        SDL_GPUTransferBuffer* tb = SDL_CreateGPUTransferBuffer(device, &tbci);
        void* ptr = SDL_MapGPUTransferBuffer(device, tb, false);
        memcpy(ptr, atlas_texture, ATLAS_WIDTH * ATLAS_HEIGHT * 4); // atlas_texture[] from atlas.inl
        SDL_UnmapGPUTransferBuffer(device, tb);

        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
        SDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cmd);
        SDL_GPUTextureTransferInfo src = {
            .transfer_buffer = tb,
            .offset = 0,
        };
        SDL_GPUTextureRegion dst = {
            .texture = atlas_gpu_texture,
            .w = ATLAS_WIDTH,
            .h = ATLAS_HEIGHT,
            .d = 1,
        };
        SDL_UploadToGPUTexture(cp, &src, &dst, false);
        SDL_EndGPUCopyPass(cp);
        SDL_SubmitGPUCommandBuffer(cmd);
        SDL_ReleaseGPUTransferBuffer(device, tb);

        // nearest neighbor sampler — pixel font looks correct
        SDL_GPUSamplerCreateInfo sci = {
            .min_filter = SDL_GPU_FILTER_NEAREST,
            .mag_filter = SDL_GPU_FILTER_NEAREST,
            .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
            .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        };
        atlas_sampler = SDL_CreateGPUSampler(device, &sci);
    }

    // init rect batches
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

void r_event(SDL_Event* e) {
    switch (e->type) {
        case SDL_EVENT_MOUSE_MOTION: mu_input_mousemove(debug_ui_context, e->motion.x, e->motion.y); break;
        case SDL_EVENT_MOUSE_WHEEL: mu_input_scroll(debug_ui_context, 0, e->wheel.y * -30); break;
        case SDL_EVENT_TEXT_INPUT: mu_input_text(debug_ui_context, e->text.text); break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            int b = button_map[e->button.button & 0xff];
            if (b && e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) { mu_input_mousedown(debug_ui_context, e->button.x, e->button.y, b); }
            if (b && e->type ==   SDL_EVENT_MOUSE_BUTTON_UP) { mu_input_mouseup(debug_ui_context, e->button.x, e->button.y, b);   }
            break;
        }

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP: {
            int c = key_map[e->key.down & 0xff];
            if (c && e->type == SDL_EVENT_KEY_DOWN) { mu_input_keydown(debug_ui_context, c); }
            if (c && e->type ==   SDL_EVENT_KEY_UP) { mu_input_keyup(debug_ui_context, c);   }
            break;
        }
    }
}

void r_load_frame(void) {
    // processes and executes all calls to microui create objs
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
        batch.capacity *= 2;
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

// texture helper
static void push_text_quad(mu_Rect src, int dx, int dy, int dw, int dh, mu_Color color) {
    if (text_batch.count >= text_batch.capacity) {
        text_batch.capacity = text_batch.capacity ? text_batch.capacity * 2 : 64;
        text_batch.quads = realloc(text_batch.quads, sizeof(TextQuad) * text_batch.capacity);
    }

    float ax = src.x / (float)ATLAS_WIDTH;
    float ay = src.y / (float)ATLAS_HEIGHT;
    float bx = (src.x + src.w) / (float)ATLAS_WIDTH;
    float by = (src.y + src.h) / (float)ATLAS_HEIGHT;

    float r = color.r / 255.0f;
    float g = color.g / 255.0f;
    float b = color.b / 255.0f;
    float a = color.a / 255.0f;

    float x0 = dx, y0 = dy;
    float x1 = dx + dw, y1 = dy + dh;
/*
    text_batch.quads[text_batch.count++] = { .verts = {
        { x0, y0,  ax, ay,  r, g, b, a },
        { x1, y0,  bx, ay,  r, g, b, a },
        { x1, y1,  bx, by,  r, g, b, a },
        { x0, y1,  ax, by,  r, g, b, a },
    }};
    */
}

void r_draw_text(const char *text, mu_Vec2 pos, mu_Color color) {
    int x = pos.x;
    for (const char* p = text; *p; p++) {
        mu_Rect src = atlas[ATLAS_FONT + (unsigned char)*p];
        push_text_quad(src, x, pos.y, src.w, src.h, color);
        x += src.w;
    }
}

void r_draw_icon(int id, mu_Rect rect, mu_Color color) {
    mu_Rect src = atlas[id];
    int x = rect.x + (rect.w - src.w) / 2;
    int y = rect.y + (rect.h - src.h) / 2;
    push_text_quad(src, x, y, src.w, src.h, color);
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


