#ifndef CLIENT_H_
#define CLIENT_H_

#include <SDL3/SDL_events.h>

int client_init();
void client_shutdown();
void client_event(SDL_Event* e);
void client_process();

#endif
