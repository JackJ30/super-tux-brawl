#include "platform.h"
#include "logger.h"

PlatformState platform;

int platformInit() {
	for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i) {
		if (strcmp(SDL_GetVideoDriver(i), "wayland") == 0) {
			SDL_SetHintWithPriority(SDL_HINT_VIDEO_DRIVER, "wayland", SDL_HINT_OVERRIDE);
		}
	}

    // init sdl
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		log_err("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
	}

    // create sdl window
    platform.width = 800;
    platform.height = 600;
    platform.window = SDL_CreateWindow("Super Tux Brawl", platform.width, platform.height, SDL_WINDOW_RESIZABLE);
    if (platform.window == NULL) {
		log_err("Failed to create SDL window: %s", SDL_GetError());
        return 1;
    }

    return 0;
}

void platformShutdown() {
    SDL_DestroyWindow(platform.window);
    SDL_Quit();
}
