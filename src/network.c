#include "network.h"
#include "enet/enet.h"
#include "logger.h"

struct NetworkServer {
    ENetAddress address;
    ENetHost* server;
};

struct NetworkClient {
    int socket_fd;
    char host[256];
};

b8 network_init(void) {
    if (enet_initialize() != 0) {
        log_err("network_init failed to initialize ENet");
        return false;
    }

    return true;
}

void network_uninit(void) {
    // other cleanup if needed

    enet_deinitialize();
}

// server
void network_server_create(NetworkServer* server, u32 port) {
    server->address.host = ENET_HOST_ANY;
    server->address.port = 0; // let OS pick
    log_info("connected to port: %d", server->address.port);


}

