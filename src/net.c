#include "net.h"

#include "util/logger.h"
#include "enet/enet.h"

/* init */

int net_init() {
    if (enet_initialize() != 0)
    {
        log_err("ENet failed to initialize.");
        return 1;
    }

    return 0;
}

void net_shutdown() {
    enet_deinitialize();
}
