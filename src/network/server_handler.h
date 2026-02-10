#ifndef SERVER_HANDLER_H_
#define SERVER_HANDLER_H_

#include "enet/enet.h"

/*
Server needs to consider for lag compensation

*/

void player_connect(ENetEvent e);
void player_disconnect(ENetEvent e);
void packet_recieved(ENetEvent e);

#endif
