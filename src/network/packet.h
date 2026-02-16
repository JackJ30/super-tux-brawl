#ifndef PACKET_H_
#define PACKET_H_

#include "../inc.h"

/*
Packet definitions, serialize and desserialization
*/

/*
* Input only sent to server, client shouldn't need to use this information
*/
typedef struct {
    // 0-127 ascii characters are 1 for on, 0 for off
    u32 input_flag[4];
    f32 mouse_x;
    f32 mouse_y;
} NetPackInput;

/*
 * Player Location
 *
*/
typedef struct {
    // player information
    u32 players;
    f32* position_x;
    f32* position_y;
} NetPackState;

/*
 * Chat
 * Characters loaded in
*/
typedef struct {
   char* chat_log; // all logs are new
} NetPackEvent;


// Each packet is a node: returned packets point to
typedef struct NetPacket {
    NetPackInput* input;
    NetPackState* state;
    NetPackEvent* event;
    struct NetPacket* next;
} NetPacket;

// readable format -> compressed network packet
byte* packet_serialize(NetPacket* data);
// compressed network packet -> readable format
NetPacket* packet_deserialize(const char* data);

void packet_destroy(NetPacket* packet);

#endif

