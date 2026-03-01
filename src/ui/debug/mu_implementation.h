#ifdef DEBUG

#include "../../deps/microui/microui.h"
#include <SDL3/SDL.h>

typedef struct {
   float x, y, w, h;
   float r, g, b;
} MURect;

typedef struct {
    MURect* rects;
    int count;
    int capacity;
} MURectBatch;

typedef struct {
    float x, y;   // position
    float u, v;   // atlas UV (normalized 0-1)
    float r, g, b, a;
} MUTextVertex;

typedef struct {
    MUTextVertex* quads[4]; // array of quads
    int count;
    int capacity;
    SDL_GPUBuffer* vertex_buffer;
    SDL_GPUTransferBuffer* transfer_buffer;
} MUTextBatch;

static MURectBatch batch = {0};
static MUTextBatch text_batch = {0};

void r_init(SDL_GPUDevice* device);
void r_uninit(void);

void r_event(SDL_Event* e);

void r_load_frame(void);
void r_clear_batch(void);
void r_draw_rect(mu_Rect rect, mu_Color color);

// todo
void r_draw_text(const char *text, mu_Vec2 pos, mu_Color color);
void r_draw_icon(int id, mu_Rect rect, mu_Color color);
int r_get_text_width(const char *text, int len);
int r_get_text_height(void);
void r_set_clip_rect(mu_Rect rect);
void r_clear(mu_Color color);
void r_present(void);

#endif
