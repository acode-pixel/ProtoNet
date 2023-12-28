#ifndef PARSER_MODULES_H
#define PARSER_MODULES_H
#include "server.h"
#include <sys/stat.h>

int brodParser(Packet* buf, Client* client, Server* serv);

#endif