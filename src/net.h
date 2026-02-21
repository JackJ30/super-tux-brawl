#ifndef NET_H_
#define NET_H_

#include "util/inc.h"

static const i32 DEFAULT_PORT = 2760;
static const i32 MAX_PEERS = 32;

/* init */
int net_init();
void net_shutdown();

#endif
