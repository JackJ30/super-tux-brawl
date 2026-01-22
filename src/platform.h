#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <SDL3/SDL.h>

typedef struct {
    SDL_Window* window;
    int width, height;
} PlatformState;

extern PlatformState platform;

int platformInit();
void platformShutdown();

#endif
