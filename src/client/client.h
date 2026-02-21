#ifndef CLIENT_H_
#define CLIENT_H_

#include <SDL3/SDL_events.h>
#include "util/inc.h"

int client_init(b8 self_host);
void client_shutdown();

void client_event(SDL_Event* e);
void client_process();

#endif
