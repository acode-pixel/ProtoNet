#include "core.h"

int setSockOpts(int sock, SocketOpt* so, char opts[]){
	so->reuseaddr = opts[0];
	so->keepalive = opts[1];
	so->dontroute = opts[2];

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opts[0], sizeof(int));
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &opts[1], sizeof(int));
	setsockopt(sock, SOL_SOCKET, SO_DONTROUTE, &opts[2], sizeof(int));
	
	return 0;
}

uint32_t getInterIP(int fd,char inter[]){
	struct ifreq ifr;
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, inter, IFNAMSIZ-1);
	if (ioctl(fd, SIOCGIFADDR, &ifr) == -1){
		perror("Failed tyo get IP of inter");
		return 0;
	} 
	return ((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr.s_addr;
}

int sendPck(int fd, char* Name, uint8_t Mode, void* data){
	Packet* pck = NULL;
	pck = (Packet*) malloc(sizeof(Packet) + strlen(data));
	memset(pck, 0, sizeof(Packet) + strlen(data));
	memcpy(pck->Proto, "SPTP", 4);
	memcpy(pck->Name, Name, 12);
	pck->Mode = Mode;
	if (strlen(data) > 1024){
		errno = 84;
		perror("Pck creation error");
		free(pck);
		return -1;
	}
	memcpy(pck->data, data, strlen(data));
	pck->datalen = strlen(data)+1;

	if (send(fd, pck, sizeof(*pck) + strlen(data)+1, 0) == -1){
		perror("Falied to send Pck");
		free(pck);
		return -1;
	}

	free(pck);
	return 0;
}

int readPck(int fd, Packet* buf){

	if (recv(fd, buf, sizeof(*buf), 0) == -1){
		perror("read Failed:");
		return errno;
	}

	if (strlen(buf->Proto) == 0){
		return -1;
	}

	buf = realloc(buf, sizeof(Packet)+buf->datalen);
	read(fd, buf->data, buf->datalen);

	return 0;
} 
