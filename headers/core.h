#ifndef CORE_H
#define CORE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/event.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

#define SPTP_BROD 1
#define SPTP_TRAC 2
#define SPTP_DATA 3

struct BROD {
	uint8_t hops;
	char fileReq[];
};

struct DATA {
	uint tracID;
	uint8_t data[1024];
};

struct TRAC {
	uint tracID;
	uint8_t hops;
	uint8_t lifetime;
};

typedef struct tracItem {
	uint tracID; 		// transaction ID
	uint8_t deleted;	// transaction is deleted
	char fileRequester[12]; // Name of file requester
	uint8_t hops; 		// hops between client and server from initial BROD packet
	uint8_t lifetime; 	// calculated lifetime of packet from hops
	void* fileOffset; 	// current file offset
	uint8_t confirmed; 	// if transaction id is confirmed
	uint8_t canDelete;	// if transaction can be deleted
	char fileReq[255]; 	// file requested

} tracItem;

typedef struct Packet {
	char Proto[4];
	char Name[12];
	uint8_t Mode;
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
int sendPck(int fd, char *Name, uint8_t Mode, void* data);
int readPck(int fd, Packet* buf);
int fillTracItem(tracItem* trac, uint tracID, char* fileRequester, uint8_t hops, uint8_t lifetime, void* fileOffset, char* fileReq);

#endif
