#ifndef TRAC_MODULES_H
#define TRAC_MODULES_H
#include "server.h"

int addTracItem(tracList* Traclist, uint tracID, uint8_t hops, uint8_t lifetime, void* fileOffset, char* fileReq);
int tracSpread(clientList* Clientlist, Packet* buf, Server* serv);

#endif