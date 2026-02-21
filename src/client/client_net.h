#ifndef CLIENT_NET_H_
#define CLIENT_NET_H_

#include "util/inc.h"

int client_net_init(char* address, i32 port);
void client_net_shutdown();

void client_net_process();

#endif
