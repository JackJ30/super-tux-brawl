#include "network.h"
#include "logger.h"

/* TODO
Libsodium - encrypts messages
Thread server ontop of game
*/

static NetworkServer server = {0};
static NetworkClient client = {0};
static b8 enet_init = false;

// ---------- SERVER ----------
i32 network_server_thread(void* data) {

    ENetEvent event;

    SDL_SetAtomicInt(&server.running, 1);
    SDL_SetAtomicInt(&server.packet_counter, 0);

    for (int i = 0; i < (int)SDL_GetAtomicInt(&server.max_clients); i++) {
        server.connected_clients[i] = NULL;
    }

    while (SDL_GetAtomicInt(&server.running)) {
        // run through queue
        // send all messages


        // recieve all mesages
        while (enet_host_service(server.server, &event, NETWORK_SERVER_POLL_TIMEOUT_MS) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT: {
                    // find slot for client
                    int slot = -1;
                    for (int i = 0; i < SDL_GetAtomicInt(&server.max_clients); i++) {
                        if (server.connected_clients[i] == NULL) {
                            slot = i;
                            break;
                        }
                    }

                    // sets user info, or rejects them
                    if (slot >= 0) {
                        server.connected_clients[slot] = event.peer;
                        event.peer->data = (void*)(size_t)slot;
                        log_info("[server] client %d connected from %x:%u",
                            slot, event.peer->address.host, event.peer->address.port
                        );
                    } else {
                        log_warn("[server] client rejected - server full");
                        enet_peer_disconnect(event.peer, 0);
                    }

                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE: {
                    int slot = (int)(size_t)event.peer->data;
                    int counter = SDL_AtomicIncRef(&server.packet_counter);

                    log_info("[server] packet #%d from client %d on channel %u (%zu bytes): %s",
                            counter, slot, event.channelID,
                            event.packet->dataLength, event.packet->data
                    );

                    // Echo back that packet(number) was recieved
                    char response[256];
                    snprintf(response, sizeof(response), "ACK: packet #%d received", counter);

                    ENetPacket* packet = enet_packet_create(response, strlen(response) + 1,
                                                            ENET_PACKET_FLAG_RELIABLE);
                    enet_peer_send(event.peer, 0, packet);

                    enet_packet_destroy(event.packet);
                    break;
                }

                case ENET_EVENT_TYPE_DISCONNECT: {
                    int slot = (int)(size_t)event.peer->data;
                    log_info("[server] client %d disconnected", slot);
                    server.connected_clients[slot] = NULL;
                    event.peer->data = NULL;
                    break;
                }

                case ENET_EVENT_TYPE_NONE:
                    default:
                        break;
            }
        }
    }

    log_info("server shutting down");
    return 0;
}

b8 network_server_create(u32 port, u32 max_clients) {
    if (SDL_GetAtomicInt(&server.running)) {
        log_warn("network_init_server already intialized server once");
        return false;
    }

    if (max_clients > NETWORK_MAX_CLIENTS) {
        log_warn("netowkr_server_create max clients capped at: %x", NETWORK_MAX_CLIENTS);
        max_clients = NETWORK_MAX_CLIENTS;
    }

    if (!enet_init) {
        if (enet_initialize() != 0) {
            log_err("network_init_server failed to initialize ENet");
            return false;
        }
        enet_init = true;
    }

    server.address.host = ENET_HOST_ANY;
    server.address.port = port;

    server.server = enet_host_create(
        &server.address,
        max_clients,
        NETWORK_MAX_CHANNELS,
        0, // bandwidth in
        0  // bandwidth out
    );

    if (server.server == NULL) {
        log_err("network_server_create failed to create ENet server host");
        if (enet_init) {
            enet_deinitialize();
            enet_init = false;
        }
        return false;
    }

    SDL_SetAtomicInt(&server.max_clients, max_clients);

    SDL_Thread* thread = SDL_CreateThread(network_server_thread, "NetworkServerThread", (void*)NULL);
    if (thread == NULL) {
        log_err("network_server_create failed to start thread: %s", SDL_GetError());
        enet_host_destroy(server.server);
        return false;
    }

    SDL_DetachThread(thread);
    log_info("network_server_create on port: %d", server.address.port);

    return true;
}

b8 network_server_destroy() {
    if (!SDL_GetAtomicInt(&server.running)) {
        log_warn("network_server_destroy never intialized the server");
        return false;
    }

    SDL_SetAtomicInt(&server.running, 0);
    SDL_Delay(100);

    if (server.server) {
        enet_host_destroy(server.server);
        server.server = NULL;
    }

    if (enet_init) {
        enet_deinitialize();
        enet_init = false;
    }

    return 1;
}

// ---------- CLIENT ----------
b8 network_client_connect(const char* ip, u32 port) {
    if (SDL_GetAtomicInt(&client.connected)) {
        log_warn("network_client_connect already connected");
        return false;
    }

    if (!enet_init) {
        if (enet_initialize() != 0) {
            log_err("network_client_connect failed to initialize ENet");
            return false;
        }
        enet_init = true;
    }

    client.client = enet_host_create(
        NULL,  // create a client host
        1,     // only allow 1 outgoing connection
        NETWORK_MAX_CHANNELS,
        0,     // bandwidth in
        0      // bandwidth out
    );

    if (client.client == NULL) {
        log_err("network_client_connect failed to create ENet client");
        return false;
    }

    ENetAddress address;
    enet_address_set_host(&address, ip);
    address.port = port;

    client.peer = enet_host_connect(client.client, &address, NETWORK_MAX_CHANNELS, 0);
    if (client.peer == NULL) {
        log_err("network_client_connect failed to initiate connection");
        enet_host_destroy(client.client);
        return false;
    }

    // Wait for connection
    ENetEvent event;
    if (enet_host_service(client.client, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT) {
        log_info("[client] connected to %s:%u", ip, port);
        SDL_SetAtomicInt(&client.connected, 1);
        return true;
    } else {
        log_err("[client] connection to %s:%u failed", ip, port);
        enet_peer_reset(client.peer);
        enet_host_destroy(client.client);
        return false;
    }
}

b8 network_client_send_packet(const char* data) {
    if (!SDL_GetAtomicInt(&client.connected)) {
        log_warn("network_client_send_packet not connected");
        return false;
    }

    ENetPacket* packet = enet_packet_create(data, strlen(data) + 1, ENET_PACKET_FLAG_RELIABLE);
    if (enet_peer_send(client.peer, 0, packet) < 0) {
        log_err("[client] failed to send packet");
        return false;
    }

    enet_host_flush(client.client);
    log_info("[client] sent packet: %s", data);
    return true;
}

void network_client_poll() {
    if (!SDL_GetAtomicInt(&client.connected)) {
        return;
    }

    ENetEvent event;
    while (enet_host_service(client.client, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                log_info("[client] received: %s", event.packet->data);
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                log_info("[client] disconnected");
                SDL_SetAtomicInt(&client.connected, 0);
                break;

            default:
                break;
        }
    }
}

b8 network_client_disconnect() {
    if (!SDL_GetAtomicInt(&client.connected)) {
        return false;
    }

    enet_peer_disconnect(client.peer, 0);

    // Wait for disconnect
    ENetEvent event;
    while (enet_host_service(client.client, &event, 3000) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                log_info("[client] disconnection succeeded");
                SDL_SetAtomicInt(&client.connected, 0);
                enet_host_destroy(client.client);
                return true;
            case ENET_EVENT_TYPE_CONNECT:
                break;
            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }

    enet_peer_reset(client.peer);
    enet_host_destroy(client.client);
    SDL_SetAtomicInt(&client.connected, 0);
    return true;
}
=======
#include "enet/enet.h"
=======
>>>>>>> 3a6162a (Finished basic network: can relay information to server, but info isnt)
#include "logger.h"

/* TODO
Libsodium - encrypts messages
Thread server ontop of game
*/

static NetworkServer server = {0};
static NetworkClient client = {0};
static b8 enet_init = false;

// ---------- SERVER ----------
i32 network_server_thread(void* data) {

    ENetEvent event;
    log_info("server started on port %d", server.address.host);

    SDL_SetAtomicInt(&server.running, 1);
    SDL_SetAtomicInt(&server.packet_counter, 0);

    for (int i = 0; i < (int)SDL_GetAtomicInt(&server.max_clients); i++) {
        server.connected_clients[i] = NULL;
    }

    while (SDL_GetAtomicInt(&server.running)) {
        // run through queue
        // send all messages


        // recieve all mesages
        while (enet_host_service(server.server, &event, NETWORK_SERVER_POLL_TIMEOUT_MS) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT: {
                    // find slot for client
                    int slot = -1;
                    for (int i = 0; i < SDL_GetAtomicInt(&server.max_clients); i++) {
                        if (server.connected_clients[i] == NULL) {
                            slot = i;
                            break;
                        }
                    }

                    // sets user info, or rejects them
                    if (slot >= 0) {
                        server.connected_clients[slot] = event.peer;
                        event.peer->data = (void*)(size_t)slot;
                        log_info("[server] client %d connected from %x:%u",
                            slot, event.peer->address.host, event.peer->address.port
                        );
                    } else {
                        log_warn("[server] client rejected - server full");
                        enet_peer_disconnect(event.peer, 0);
                    }

                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE: {
                    int slot = (int)(size_t)event.peer->data;
                    int counter = SDL_AtomicIncRef(&server.packet_counter);

                    log_info("[server] packet #%d from client %d on channel %u (%zu bytes): %s",
                            counter, slot, event.channelID,
                            event.packet->dataLength, event.packet->data);

                    // Echo back the counter to the client
                    char response[256];
                    snprintf(response, sizeof(response), "ACK: packet #%d received", counter);

                    ENetPacket* packet = enet_packet_create(response, strlen(response) + 1,
                                                            ENET_PACKET_FLAG_RELIABLE);
                    enet_peer_send(event.peer, 0, packet);

                    enet_packet_destroy(event.packet);
                    break;
                }

                case ENET_EVENT_TYPE_DISCONNECT: {
                    int slot = (int)(size_t)event.peer->data;
                    log_info("[server] client %d disconnected", slot);
                    server.connected_clients[slot] = NULL;
                    event.peer->data = NULL;
                    break;
                }

                case ENET_EVENT_TYPE_NONE:
                    default:
                        break;
            }
        }
    }

    log_info("server shutting down");
    return 0;
}

b8 network_server_create(u32 port, u32 max_clients) {
    if (SDL_GetAtomicInt(&server.running)) {
        log_warn("network_init_server already intialized server once");
        return false;
    }

    if (max_clients > NETWORK_MAX_CLIENTS) {
        log_warn("netowkr_server_create max clients capped at: %x", NETWORK_MAX_CLIENTS);
        max_clients = NETWORK_MAX_CLIENTS;
    }

    if (!enet_init) {
        if (enet_initialize() != 0) {
            log_err("network_init_server failed to initialize ENet");
            return false;
        }
        enet_init = true;
    }

    server.address.host = ENET_HOST_ANY;
    server.address.port = port;

    server.server = enet_host_create(
        &server.address,
        max_clients,
        NETWORK_MAX_CHANNELS,
        0, // bandwidth in
        0  // bandwidth out
    );

    if (server.server == NULL) {
        log_err("network_server_create failed to create ENet server host");
        if (enet_init) {
            enet_deinitialize();
            enet_init = false;
        }
        return false;
    }

    SDL_SetAtomicInt(&server.max_clients, max_clients);

    SDL_Thread* thread = SDL_CreateThread(network_server_thread, "NetworkServerThread", (void*)NULL);
    if (thread == NULL) {
        log_err("network_server_create failed to start thread: %s", SDL_GetError());
        enet_host_destroy(server.server);
        return false;
    }

    SDL_DetachThread(thread);
    log_info("network_server_create on port: %d", server.address.port);

    return true;
}

b8 network_server_destroy() {
    if (!SDL_GetAtomicInt(&server.running)) {
        log_warn("network_server_destroy never intialized the server");
        return false;
    }

    SDL_SetAtomicInt(&server.running, 0);
    SDL_Delay(100);

    if (server.server) {
        enet_host_destroy(server.server);
        server.server = NULL;
    }

    if (enet_init) {
        enet_deinitialize();
        enet_init = false;
    }

    return 1;
}

// server
void network_server_create(NetworkServer* server, u32 port) {
    server->address.host = ENET_HOST_ANY;
    server->address.port = 0; // let OS pick
    log_info("connected to port: %d", server->address.port);
// ---------- CLIENT ----------
// b8 network_client_connect(const char* ip, u32 port) {
//     if (SDL_GetAtomicInt(&client.connected)) {
//         log_warn("network_client_connect already connected");
//         return false;
//     }
//
    if (!enet_init) {
        if (enet_initialize() != 0) {
            log_err("network_client_connect failed to initialize ENet");
            return false;
        }
        enet_init = true;
    }

    client.client = enet_host_create(
        NULL,  // create a client host
        1,     // only allow 1 outgoing connection
        NETWORK_MAX_CHANNELS,
        0,     // bandwidth in
        0      // bandwidth out
    );

    if (client.client == NULL) {
        log_err("network_client_connect failed to create ENet client");
        return false;
    }

    ENetAddress address;
    enet_address_set_host(&address, ip);
    address.port = port;

    client.peer = enet_host_connect(client.client, &address, NETWORK_MAX_CHANNELS, 0);
    if (client.peer == NULL) {
        log_err("network_client_connect failed to initiate connection");
        enet_host_destroy(client.client);
        return false;
    }

    // Wait for connection
    ENetEvent event;
    if (enet_host_service(client.client, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT) {
        log_info("[client] connected to %s:%u", ip, port);
        SDL_SetAtomicInt(&client.connected, 1);
        return true;
    } else {
        log_err("[client] connection to %s:%u failed", ip, port);
        enet_peer_reset(client.peer);
        enet_host_destroy(client.client);
        return false;
    }
}

b8 network_client_send_packet(const char* data) {
    if (!SDL_GetAtomicInt(&client.connected)) {
        log_warn("network_client_send_packet not connected");
        return false;
    }

    ENetPacket* packet = enet_packet_create(data, strlen(data) + 1, ENET_PACKET_FLAG_RELIABLE);
    if (enet_peer_send(client.peer, 0, packet) < 0) {
        log_err("[client] failed to send packet");
        return false;
    }

    enet_host_flush(client.client);
    log_info("[client] sent packet: %s", data);
    return true;
}

void network_client_poll() {
    if (!SDL_GetAtomicInt(&client.connected)) {
        return;
    }

    ENetEvent event;
    while (enet_host_service(client.client, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                log_info("[client] received: %s", event.packet->data);
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                log_info("[client] disconnected");
                SDL_SetAtomicInt(&client.connected, 0);
                break;

            default:
                break;
        }
    }
}

b8 network_client_disconnect() {
    if (!SDL_GetAtomicInt(&client.connected)) {
        return false;
    }

    enet_peer_disconnect(client.peer, 0);

    // Wait for disconnect
    ENetEvent event;
    while (enet_host_service(client.client, &event, 3000) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                log_info("[client] disconnection succeeded");
                SDL_SetAtomicInt(&client.connected, 0);
                enet_host_destroy(client.client);
                return true;
            case ENET_EVENT_TYPE_CONNECT:
                break;
            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }

    enet_peer_reset(client.peer);
    enet_host_destroy(client.client);
    SDL_SetAtomicInt(&client.connected, 0);
    return true;
}


