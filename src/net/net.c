#include "net.h"

#include "util/logger.h"
#include "enet/enet.h"

int net_start(b8 server, b8 client) {
    return 0;
}

void net_finish() {
}

/* struct { */
/*     ENetHost* host; */
/*     ENetAddress address; */
/*     ENetPeer* peer; */
/*     b8 connected; */
/* } client = {0}; */

/* int client_init() { */

/*     /\* initialize *\/ */
/*     if (enet_initialize () != 0) */
/*     { */
/*         log_err("ENet failed to initialize."); */
/*         return 1; */
/*     } */

/*     /\* start client *\/ */
/*     client.host = enet_host_create(NULL, 1, 2, 0, 0); */
/*     if (client.host == NULL) { */
/*         log_err("ENet failed to create client."); */
/*         return 1; */
/*     } */

/*     /\* start connection to server *\/ */
/*     enet_address_set_host (&client.address, "localhost"); */
/*     client.address.port = PORT; */
/*     client.peer = enet_host_connect (client.host, &client.address, 2, 0); */
/*     if (client.peer == NULL) { */
/*         log_err("No available peers for initiating an ENet connection."); */
/*         return 1; */
/*     } else { */
/*         log_info("Attempting to connect to server..."); */
/*     } */

/*     /\* wait for server connection *\/ */
/*     ENetEvent e; */
/*     if (enet_host_service (client.host, &e, 5000) > 0 && e.type == ENET_EVENT_TYPE_CONNECT) */
/*     { */
/*         log_info("Connection succeeded."); */
/*         client.connected = true; */
/*     } else { */
/*         log_info("Failed to connect to server."); */
/*         enet_peer_reset(client.peer); */
/*         return 1; */
/*     } */

/*     return 0; */
/* } */

/* void client_shutdown() { */

/*     if (client.connected) { */
/*         // disconnect */
/*         log_info("Attempting to disconnect from server..."); */
/*         enet_peer_disconnect(client.peer, 0); */

/*         // wait for disconnection */
/*         ENetEvent e; */
/*         while (enet_host_service(client.host, &e, 1000) > 0) */
/*         { */
/*             if (e.type == ENET_EVENT_TYPE_DISCONNECT) { */
/*                 log_info("Disconnected."); */
/*                 break; */
/*             } else if (e.type == ENET_EVENT_TYPE_RECEIVE) { */
/*                 enet_packet_destroy(e.packet); */
/*             } */
/*         } */
/*         enet_peer_reset(client.peer); */
/*     } */

/*     enet_host_destroy(client.host); */
/*     enet_deinitialize(); */
/* } */

/* void client_process() { */
/*     ENetEvent e; */
/*     while (enet_host_service(client.host, &e, 0) > 0) { */
/*         switch (e.type) { */
/*             case ENET_EVENT_TYPE_RECEIVE: */
/*                 log_info ("A packet of length %zu containing %s was received from %s on channel %u.", */
/*                           e.packet->dataLength, */
/*                           e.packet->data, */
/*                           (char*)e.peer->data, */
/*                           e.channelID); */

/*                 /\* Clean up the packet now that we're done using it. *\/ */
/*                 enet_packet_destroy (e.packet); */
/*                 break; */

/*             case ENET_EVENT_TYPE_DISCONNECT: */
/*                 log_info("Disconnected from server."); */
/*                 client.connected = false; */
/*                 break; */

/*             case ENET_EVENT_TYPE_CONNECT: */
/*             case ENET_EVENT_TYPE_NONE: */
/*                 break; */
/*         } */
/*     } */

/* } */

/* void client_send_input(Input* input) { */

/* } */

/* struct { */
/*     ENetHost* host; */
/*     ENetAddress address; */
/* } server = {0}; */

/* int server_init() { */
/*     /\* initialize *\/ */
/*     if (enet_initialize () != 0) */
/*     { */
/*         log_err("ENet failed to initialize."); */
/*         return 1; */
/*     } */

/*     /\* start server *\/ */
/*     server.address.host = ENET_HOST_ANY; */
/*     server.address.port = PORT; */
/*     server.host = enet_host_create(&server.address, MAX_PEERS, 2, 0, 0); */
/*     if (server.host == NULL) { */
/*         log_err("ENet failed to create server."); */
/*         return 1; */
/*     } */
/*     log_info("Started server."); */

/*     return 0; */
/* } */

/* void server_shutdown() { */
/*     // disconnect all peers */
/*     for (size i = 0; i < server.host->peerCount; ++i) { */
/*         enet_peer_disconnect(&server.host->peers[i], 0); */
/*     } */
/*     enet_host_flush(server.host); */
/*     log_info("Disconnected peers."); */

/*     enet_host_destroy(server.host); */
/*     enet_deinitialize(); */
/* } */

/* void server_process() { */
/*     ENetEvent e; */
/*     while (enet_host_service(server.host, &e, 0) > 0) { */
/*         switch (e.type) { */
/*             case ENET_EVENT_TYPE_CONNECT: */
/*                 log_info ("A new client connected from %x:%u.", e.peer->address.host, e.peer->address.port); */

/*                 /\* Store any relevant client information here. *\/ */
/*                 e.peer->data = "Client information"; */

/*                 break; */

/*             case ENET_EVENT_TYPE_DISCONNECT: */
/*                 log_info ("%x:%u disconnected.", e.peer->address.host, e.peer->address.port); */

/*                 /\* Reset the peer's client information. *\/ */
/*                 e.peer -> data = NULL; */
/*                 break; */

/*             case ENET_EVENT_TYPE_RECEIVE: */
/*                 log_info ("A packet of length %zu containing %s was received from %s on channel %u.", */
/*                           e.packet->dataLength, */
/*                           e.packet->data, */
/*                           (char*)e.peer->data, */
/*                           e.channelID); */

/*                 /\* Clean up the packet now that we're done using it. *\/ */
/*                 enet_packet_destroy (e.packet); */

/*                 break; */

/*             case ENET_EVENT_TYPE_NONE: */
/*                 break; */
/*         } */
/*     } */
/* } */
