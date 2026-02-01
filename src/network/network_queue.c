#include "network_queue.h"
#include "../logger.h"
#include <stdlib.h>

NetQueue* nq_init() {
    NetQueue* queue = (NetQueue*)malloc(sizeof(NetQueue));

    queue->head = NULL;
    queue->tail = NULL;
    queue->mutex = SDL_CreateMutex();
    queue->condition = SDL_CreateCondition();

    // no mutex, no queue
    if (!queue->mutex || !queue->condition) {
        if (queue->mutex) SDL_DestroyMutex(queue->mutex);
        if (queue->condition) SDL_DestroyCondition(queue->condition);
        free(queue);
        return NULL;
    }

    return queue;
}

void nq_destroy(NetQueue* queue) {
    if (!queue) return;

    // locks before dumping
    SDL_LockMutex(queue->mutex);

    // Free all packets
    NetPacket* node = queue->head;
    while (node != NULL) {
        NetPacket* next = node->next;
        free(node);
        node = next;
    }

    SDL_UnlockMutex(queue->mutex);

    SDL_DestroyMutex(queue->mutex);
    SDL_DestroyCondition(queue->condition);

    free(queue);
}

NetPacket nq_pop(NetQueue* queue) {
    SDL_LockMutex(queue->mutex);

    while (queue->head == NULL) {
        SDL_WaitCondition(queue->condition, queue->mutex);
    }

    // get front packet
    NetPacket* node = queue->head;
    NetPacket packet;
    memcpy(&packet, node, sizeof(NetPacket));

    // remove from queue
    queue->head = node->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    SDL_UnlockMutex(queue->mutex);

    // cleanup
    free(node);
    return packet;
}

void nq_push(NetQueue* queue, NetPacket* packet) {
    if (packet == NULL || queue == NULL) {
        log_warn("nq_push packet or queue are NULL, please pass valid data");
        return;
    }

    SDL_LockMutex(queue->mutex);

    // ensure next packet is null
    if (packet->next != NULL) {
        log_warn("nq_push packets added to queue must not have 'next' element, please don't pre init the list yourself");
        return;
    }

    // new queue
    if (queue->head == NULL && queue->tail == NULL) {
        queue->head = queue->tail = packet;
    } else {
        queue->tail->next = packet;
        queue->tail = packet;
    }

    SDL_SignalCondition(queue->condition);
    SDL_UnlockMutex(queue->mutex);
}





