#ifndef CLIENT_H
#define CLIENT_H

#include "core.h"
#define C_PORT 5657

typedef struct Client {
	char name[12];
	int Socket;
	int socketMode;
} Client;

Client* Cl_Init(char* inter, char name[]);

int connectToNetwork(char* IP, Client* cli);
int makeFileReq(int fd, char* Name, char File[]);
#endif
