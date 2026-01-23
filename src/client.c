#include <stdio.h>
#include <enet/enet.h>
#include <string.h>
 
int 
main (int argc, char ** argv) 
{
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    ENetHost* client;
     
    client = enet_host_create(NULL /* create a client host */,
                1 /* only allow 1 outgoing connection */,
                2 /* allow up 2 channels to be used, 0 and 1 */,
                0 /* assume any amount of incoming bandwidth */,
                0 /* assume any amount of outgoing bandwidth */);
     
    if (!client) {
        fprintf(stderr, "An error occurred while trying to create an ENet client host.\n");
        return 1;
    }
    ENetAddress address;
    ENetEvent event;
    ENetPeer* peer;

    /* Connect to some.server.net:1234. */
    enet_address_set_host(&address, "localhost");
    address.port = 8080;

    /* Initiate the connection, allocating the two channels 0 and 1. */
    peer = enet_host_connect(client, &address, 2, 0);    

    if (peer == NULL)
    {
        fprintf(stderr, "No available peers for initiating an ENet connection.\n");
        return 1;
    }

    /* Wait up to 5 seconds for the connection attempt to succeed. */
    if (enet_host_service(client, &event, 5000) > 0 &&
            event.type == ENET_EVENT_TYPE_CONNECT) {
        puts("Connection to localhost:8080 succeeded.");

        ENetPacket * packet = enet_packet_create("packet", 
                strlen ("packet") + 1, 
                ENET_PACKET_FLAG_RELIABLE);

        /* Extend the packet so and append the string "foo", so it now */
        /* contains "packetfoo\0"                                      */
        enet_packet_resize(packet, strlen ("packetfoo") + 1);
        strcpy (&packet->data[strlen("packet")], "foo");

        /* Send the packet to the peer over channel id 0. */
        /* One could also broadcast the packet by         */
        /* enet_host_broadcast (host, 0, packet);         */
        enet_peer_send(peer, 0, packet);
        /* One could just use enet_host_service() instead. */
        enet_host_flush(client);
        enet_peer_disconnect(peer, 0);
    }
    else
    {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset(peer);
        puts("Connection to localhost:8080 failed.");
    }

    enet_host_destroy(client);
    enet_deinitialize();
}
