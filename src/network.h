#ifndef _NETWORK_H_
#define _NETWORK_H_

#define NETWORK_MAX_CLIENTS 8
#define NETWORK_MAX_CHANNELS 8
#define NETWORK_MAX_PLAYLOAD 1024

#include "inc.h"

typedef struct NetworkServer NetworkServer;
typedef struct NetworkClient NetworkClient;

b8 network_init(void);
void network_uninit(void);

// Server
void network_server_create(NetworkServer* server, u32 port);



// NetworkServer* network_server_create(short port);
// void network_server_destroy(NetworkServer* server);
// void network_server_poll(NetworkServer* server, int timeout_ms);
// void network_server_broadcast(NetworkServer* server, int channel, const void* data, int size, int reliable);
// void network_server_send(NetworkServer* server, int client_id, int channel, const void* data, int size, int reliable);
//
// // Client
// NetworkClient* network_client_create(void);
// void network_client_destroy(NetworkClient* client);
// int network_client_connect(NetworkClient* client, const char* host, int port, int timeout_ms);
// void network_client_disconnect(NetworkClient* client);
// void network_client_poll(NetworkClient* client, int timeout_ms);
// void network_client_send(NetworkClient* client, int channel, const void* data, int size, int reliable);
//
// // Events
// typedef enum {
// 	NETWORK_EVENT_NONE,
// 	NETWORK_EVENT_CONNECT,
// 	NETWORK_EVENT_CONNECTDISCONNECT,
// 	NETWORK_EVENT_CONNECTRECIEVE
// } NetworkEventType;
//
// typedef struct {
// 	NetworkEventType type;
// 	int peer_id;
// 	int channel;
// 	const void* data;
// 	int size;
// } NetworkEvent;
//
// int network_next_event(NetworkEvent* out);

#endif
