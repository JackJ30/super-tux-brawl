#include "server.h"

#include "util/logger.h"
#include "enet/enet.h"
#include "net.h"

#include <SDL3/SDL_thread.h>

int server_thread();

static struct {
    ENetHost* host;
} net = {0};

static struct {
    SDL_Thread* thread;
    SDL_AtomicInt is_running;
} server = {0};

int server_start(i32 port) {

    /* start server */
    ENetAddress net_address;
    net_address.host = ENET_HOST_ANY;
    net_address.port = port != 0 ? port : DEFAULT_PORT;
    net.host = enet_host_create(&net_address, MAX_PEERS, 2, 0, 0);
    if (net.host == NULL) {
        log_err("Server: ENet failed to create host.");
        return 1;
    }

    server.thread = SDL_CreateThread(server_thread, "Server Thread", NULL);
    if (server.thread == NULL) {
        log_err("Server: Failed to create thread: %s", SDL_GetError());
        return 1;
    }

    log_info("Server: Started.");
    return 0;
}

void server_finish() {
    /* join server thread */
    SDL_SetAtomicInt(&server.is_running, 0);
    SDL_WaitThread(server.thread, NULL);

    // disconnect all peers
    log_info("Server: disconnecting remaining peers...");
    for (size i = 0; i < net.host->peerCount; ++i) {
        enet_peer_disconnect(&net.host->peers[i], 0);
    }
    enet_host_flush(net.host);

    enet_host_destroy(net.host);
    log_info("Server: shut down.");
}

int server_thread() {
    SDL_SetAtomicInt(&server.is_running, 1);

    while (SDL_GetAtomicInt(&server.is_running) == 1) {
        ENetEvent e;
        while (enet_host_service(net.host, &e, 0) > 0) {
            switch (e.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    log_info ("Server: A new client connected from %x:%u.", e.peer->address.host, e.peer->address.port);

                    /* Store any relevant client information here. */
                    e.peer->data = "Client information";

                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    log_info("Server: %x:%u disconnected.", e.peer->address.host, e.peer->address.port);

                    /* Reset the peer's client information. */
                    e.peer -> data = NULL;
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    log_info("Server: A packet of length %zu containing %s was received from %s on channel %u.",
                              e.packet->dataLength,
                              e.packet->data,
                              (char*)e.peer->data,
                              e.channelID);

                    /* Clean up the packet now that we're done using it. */
                    enet_packet_destroy (e.packet);

                    break;

                case ENET_EVENT_TYPE_NONE:
                    break;
            }
        }
    }

    return 0;
}
