#ifndef _NETWORK_H_
#define _NETWORK_H_

#define NETWORK_MAX_CLIENTS 8
#define NETWORK_MAX_CHANNELS 2
#define NETWORK_SERVER_PORT 2760
#define NETWORK_SERVER_POLL_TIMEOUT_MS 100

#include "inc.h"
#include "enet/enet.h"
#include <SDL3/SDL.h>

/*
Network Redundancy: 32 packets of history
Network puts packets on queue
Simulation/Physics Engine removes packets from queue

*/

typedef struct {
    // ENet Required Info
    ENetAddress address;
    ENetHost* server;
    // Running Thread
    SDL_Thread* thread;
    SDL_AtomicInt running;
    // Server Info
    SDL_AtomicInt max_clients;
    SDL_AtomicInt packet_counter;
    ENetPeer* connected_clients[NETWORK_MAX_CLIENTS];
} NetworkServer;

typedef struct {
    ENetHost* client;
    ENetPeer* peer;
    SDL_AtomicInt running;
    SDL_AtomicInt connected;
} NetworkClient;

// should have server and client packet
// TODO: make some packet with data to be sent every tick

// Server
b8 network_server_create(u32 port, u32 max_client);
b8 network_server_destroy(void);

// Client
b8 network_client_connect(const char* ip, u32 port);
b8 network_client_disconnect(void);
b8 network_client_send_packet(const char* data);
void network_client_poll(void);




// Packet Queue
// void network_packet_queue_init();
// b8 network_packet_queue_empty();
// b8 network_packet_queue_full();
// b8 network_packet_queue_push();
// b8 network_packet_queue_pop();

#endif
