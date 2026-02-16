#ifndef NET_H_
#define NET_H_

#include "inc.h"

int net_init(b8 server);
void net_shutdown();
void net_process();

#endif
