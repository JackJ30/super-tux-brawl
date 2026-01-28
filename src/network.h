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

<<<<<<< HEAD
Network Layout
- Different Channels for different data
    - Input Channel: data on buttons pressed, unreliable
    - State Channel: current snapshot of all inputs
        - https://www.ggpo.net/ could help to reduce lag, used in other fighting games
        - authoritative states sent(interpolated by client to reduce visual issues)
            - packets hold position, velocity
    - Event Channel: small channel for information like chat, ui, etc. reliable because
    its useful info, but not time sensitive
    - History Buffer: rollback for previous/future states so sent data can be delivered to
    other used before they reach that visual state in game
        - lag comes with this, might have render set back 100ms so packet can be recieved by
        other player so you and the other client are updated at once
- Have a tick rate for number of packets sent, also data for packets to see if unreliable packet
should be trashed if old data
- Security: libsodium useful for protecting data to prevent cheating
- Network recieves data -> queue and is processed by some engine -> back to all users
- Client queues data from server -> buffer of packets -> game engine only renders first
half of the buffer so it has scenes to render without waiting for server
=======
>>>>>>> 3a6162a (Finished basic network: can relay information to server, but info isnt)
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
