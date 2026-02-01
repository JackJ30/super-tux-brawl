#ifndef NETWORK_QUEUE_
#define NETWORK_QUEUE_

#include "../inc.h"
#include "packet.h"
#include <SDL3/SDL.h>

/*
This file creates queues to hold all network files for client or server

Queue is thread safe
*/

typedef struct {
    NetPacket* head;
    NetPacket* tail;
    SDL_Mutex* mutex;
    SDL_Condition* condition;
} NetQueue;

NetQueue* nq_create(NetQueue* queue);
void nq_destroy(NetQueue* queue);
NetPacket nq_pop(NetQueue* queue); // cleans up in netqueue, returns a copy of the struct
void nq_push(NetQueue* queue, NetPacket* packet);

#endif
