#include "ui.h"
#include "debug/mu_implementation.h"

void ui_init(void) {
#ifdef DEBUG
    r_init();
#endif
}

void ui_uninit(void) {

}

void ui_event(SDL_Event* e) {
#ifdef DEBUG
    r_event(e);
#endif

}
