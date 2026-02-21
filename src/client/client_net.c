#include "client_net.h"

#include "enet/enet.h"
#include "util/logger.h"
#include "net.h"

static struct {
    ENetHost* host;
    ENetPeer* peer;
    b8 connected;
} net = {0};

int client_net_init(char* address, i32 port) {

    /* create host */
    net.host = enet_host_create(NULL, 1, 2, 0, 0);
    if (net.host == NULL) {
        log_err("ENet failed to create client.");
        return 1;
    }

    /* create peer and attempt connection */
    ENetAddress net_address;
    enet_address_set_host (&net_address, address);
    net_address.port = port != 0 ? port : DEFAULT_PORT;
    net.peer = enet_host_connect (net.host, &net_address, 2, 0);
    if (net.peer == NULL) {
        log_err("Client: No available peers for initiating an ENet connection.");
        return 1;
    } else {
        log_info("Client: Attempting to connect to server...");
    }

    /* wait for server connection */
    ENetEvent e;
    if (enet_host_service(net.host, &e, 5000) > 0 && e.type == ENET_EVENT_TYPE_CONNECT)
    {
        log_info("Client: Connection succeeded.");
        net.connected = true;
    } else {
        log_info("Client: Failed to connect to server.");
        enet_peer_reset(net.peer);
        return 1;
    }

    return 0;
}

void client_net_shutdown() {

    if (net.connected) {
        // disconnect
        log_info("Client: Attempting to disconnect from server...");
        enet_peer_disconnect(net.peer, 0);

        // wait for disconnection
        ENetEvent e;
        while (enet_host_service(net.host, &e, 1000) > 0)
        {
            if (e.type == ENET_EVENT_TYPE_DISCONNECT) {
                log_info("Client: disconnection success.");
                break;
            } else if (e.type == ENET_EVENT_TYPE_RECEIVE) {
                enet_packet_destroy(e.packet);
            }
        }
    }

    enet_peer_reset(net.peer);
    enet_host_destroy(net.host);
}

void client_net_process() {

    ENetEvent e;
    while (enet_host_service(net.host, &e, 0) > 0) {
        switch (e.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                log_info ("Client: A packet of length %zu containing %s was received from %s on channel %u.",
                          e.packet->dataLength,
                          e.packet->data,
                          (char*)e.peer->data,
                          e.channelID);

                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy (e.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                log_info("Client: disconnected from server.");
                net.connected = false;
                break;

            case ENET_EVENT_TYPE_CONNECT:
            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }
}
