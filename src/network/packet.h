#ifndef PACKET_H_
#define PACKET_H_

#include "../inc.h"

/*
Packet definitions and serialization
*/

typedef enum {
    // key inputs: ability, movement etc
    NetPack_INPUT,
    // current location of all characters, ability
    NetPack_STATE,
    // chat message, ui, etc
    NetPack_EVENT
} NetworkPacketType;

typedef struct NetPacket {
    NetworkPacketType type;
    char* data;
    u32 size;
    struct NetPacket* next;
} NetPacket;

// readable format -> compressed network packet
byte* packet_serialize(void* data);
// compressed network packet -> readable format
NetPacket* packet_deserialize(const char* data);

void packet_destroy(NetPacket* packet);

#endif

