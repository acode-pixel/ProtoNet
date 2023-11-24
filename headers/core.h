#ifndef CORE_H
#define CORE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

struct BROD {
	uint8_t hops;
	char fileReq[];
};

typedef struct Packet {
	char Proto[4];
	uint32_t IP;
	char Mode[1];
	uint32_t datalen;

	uint8_t data[]; /* MAX 1024 */
} Packet;

typedef struct SocketOpt {
	char reuseaddr;
	char keepalive;
	char dontroute;
} SocketOpt;

int setSockOpts(int sock, SocketOpt* so, char opts[]);
uint32_t getInterIP(int fd,char inter[]);
int sendPck(int fd, uint32_t IP, int Mode, void* data);

#endif
