#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <enet/enet.h>

int main() {
    if (enet_initialize() != 0) {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return 1;
    }
    ENetAddress address;
    ENetHost* server;

    address.host = ENET_HOST_ANY;
    address.port = 8080;

    server = enet_host_create(&address, 32, 2, 0, 0);
    if (!server) {
        fprintf(stderr, "host creation failed\n");
        return 1;
    }

    ENetEvent event;
    int clients = 0;

    /* Wait up to 1000 milliseconds for an event. */
    while (enet_host_service(server, &event, 10000) > 0)
    {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                printf("A new client connected from %x:%u.\n",
                        event.peer->address.host,
                        event.peer->address.port);

                /* Store any relevant client information here. */
                event.peer->data = &clients;
                clients++;

                break;

            case ENET_EVENT_TYPE_RECEIVE:
                /*
                printf("A packet of length %u containing %s was received from %s on channel %u.\n",
                        event.packet->dataLength,
                        event.packet->data,
                        event.peer->data,
                        event.channelID);
                */


                printf("%s\n", event.packet->data);
                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy(event.packet);

                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("%s disconnected.\n", event.peer -> data);

                /* Reset the peer's client information. */

                event.peer->data = NULL;
        }
    }
    enet_host_destroy(server);
    enet_deinitialize();
}
