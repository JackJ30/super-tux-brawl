#include "net.h"

#include "util/logger.h"
#include "enet/enet.h"

struct {
    ENetHost* host;
    ENetAddress address;
    b8 is_server;

    /* client */
    ENetPeer* peer;
} net = {0};

const i32 PORT = 2760;
const i32 MAX_PEERS = 32;

int net_init(b8 server) {
    /* initialize */
    if (enet_initialize () != 0)
    {
        log_err("ENet failed to initialize.");
        return 1;
    }

    net.is_server = server;
    if (net.is_server) {
        /* start server */
        net.address.host = ENET_HOST_ANY;
        net.address.port = PORT;
        net.host = enet_host_create(&net.address, MAX_PEERS, 2, 0, 0);
        if (net.host == NULL) {
            log_err("ENet failed to create server.");
            return 1;
        }
        log_info("Started server.");

    } else {
        /* start client */
        net.host = enet_host_create(NULL, 1, 2, 0, 0);
        if (net.host == NULL) {
            log_err("ENet failed to create client.");
            return 1;
        }

        /* start connection to server */
        enet_address_set_host (&net.address, "localhost");
        net.address.port = PORT;
        net.peer = enet_host_connect (net.host, &net.address, 2, 0);
        if (net.peer == NULL) {
            log_err("No available peers for initiating an ENet connection.");
            return 1;
        } else {
            log_info("Attempting to connect to server...");
        }

        /* wait for server connection */
        ENetEvent e;
        if (enet_host_service (net.host, &e, 5000) > 0 && e.type == ENET_EVENT_TYPE_CONNECT)
        {
            log_info("Connection succeeded.");
        } else {
            log_info("Failed to connect to server.");
            enet_peer_reset(net.peer);
            return 1;
        }
    }

    return 0;
}

void net_shutdown() {
    if (net.is_server) {

        // disconnect all peers
        for (size i = 0; i < net.host->peerCount; ++i) {
            enet_peer_disconnect(&net.host->peers[i], 0);
        }
        enet_host_flush(net.host);
        log_info("Disconnected peers.");

    } else {
        // disconnect
        log_info("Attempting to disconnect from server...");
        enet_peer_disconnect(net.peer, 0);

        // wait for disconnection
        ENetEvent e;
        while (enet_host_service(net.host, &e, 1000) > 0)
        {
            if (e.type == ENET_EVENT_TYPE_DISCONNECT) {
                log_info("Disconnected.");
                break;
            } else if (e.type == ENET_EVENT_TYPE_RECEIVE) {
                enet_packet_destroy(e.packet);
            }
        }
        enet_peer_reset(net.peer);
    }

    enet_host_destroy(net.host);
    enet_deinitialize();
}

void process_server();
void process_client();

void net_process() {
    if (net.is_server) {
        process_server();
    } else {
        process_client();
    }
}

void process_server() {
    ENetEvent e;
    while (enet_host_service(net.host, &e, 0) > 0) {
        switch (e.type) {
            case ENET_EVENT_TYPE_CONNECT:
                log_info ("A new client connected from %x:%u.", e.peer->address.host, e.peer->address.port);

                /* Store any relevant client information here. */
                e.peer -> data = "Client information";

                break;

            case ENET_EVENT_TYPE_RECEIVE:
                log_info ("A packet of length %zu containing %s was received from %s on channel %u.",
                          e.packet->dataLength,
                          e.packet->data,
                          (char*)e.peer->data,
                          e.channelID);

                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy (e.packet);

                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                log_info ("%x:%u disconnected.", e.peer->address.host, e.peer->address.port);


                /* Reset the peer's client information. */
                e.peer -> data = NULL;
                break;

            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }
}

void process_client() {
    ENetEvent e;
    while (enet_host_service(net.host, &e, 0) > 0) {
        switch (e.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                log_info ("A packet of length %zu containing %s was received from %s on channel %u.",
                          e.packet->dataLength,
                          e.packet->data,
                          (char*)e.peer->data,
                          e.channelID);

                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy (e.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                log_info("Disconnected from server.");
                break;

            case ENET_EVENT_TYPE_CONNECT:
            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }
}
