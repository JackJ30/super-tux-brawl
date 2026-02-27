#ifndef MU_IMPLEMENTATION_
#define MU_IMPLEMENTATION_

#include "../../deps/microui/microui.h"

typedef struct {
   float x, y, w, h;
   float r, g, b;
} MURect;

typedef struct {
    MURect* rects;
    int count;
    int capacity;
} MURectBatch;

static MURectBatch batch = {0};

void r_init(void);
void r_uninit(void);
void r_clear_batch(void);
void r_draw_rect(mu_Rect rect, mu_Color color);
void r_draw_text(const char *text, mu_Vec2 pos, mu_Color color);
void r_draw_icon(int id, mu_Rect rect, mu_Color color);
int r_get_text_width(const char *text, int len);
int r_get_text_height(void);
void r_set_clip_rect(mu_Rect rect);
void r_clear(mu_Color color);
void r_present(void);

#endif
