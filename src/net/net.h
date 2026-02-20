#ifndef NET_H_
#define NET_H_

#include "util/inc.h"

static const i32 PORT = 2760;
static const i32 MAX_PEERS = 32;

int net_start(b8 server, b8 client);
void net_finish();

/* server */
/* int server_init(); */
/* void server_shutdown(); */
/* void server_process(); */

/* client */
/* int client_init(); */
/* void client_shutdown(); */
/* void client_process(); */
/* void client_send_input(Input* input); */

#endif
