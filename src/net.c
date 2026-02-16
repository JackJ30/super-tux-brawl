#include "net.h"

#include "logger.h"
#include "enet/enet.h"

struct {
    ENetAddress address;
    ENetHost* host;
} net_data;

int net_init(b8 server) {
    if (enet_initialize () != 0)
    {
        log_err("ENet failed to initialize.");
        return 1;
    }

    if (server) {
        /* server */
        net_data.address.host = ENET_HOST_ANY;
        net_data.address.port = 2760;
        net_data.host = enet_host_create (&net_data.address, 32, 2, 0, 0);
        if (net_data.host == NULL) {
            log_err("ENet failed to create server.");
            return 1;
        }
    } else {
        /* client */
        net_data.host = enet_host_create (NULL, 1, 2, 0, 0);
        if (net_data.host == NULL) {
            log_err("ENet failed to create client.");
            return 1;
        }
    }

    return 0;
}

void net_shutdown() {
    enet_host_destroy(net_data.host);
    enet_deinitialize();
}
