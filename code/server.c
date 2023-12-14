#include "server.h"

Server* Init(char* inter, char* ip, char* serverName, char Dir[]){
	// Check if Dir is NULL
	assert(Dir != NULL);
	if (access(Dir, R_OK) == -1){
		return NULL;
	}
	
	// alloc server 
	Server* serv = (Server*) malloc(sizeof(Server) + strlen(Dir));
	serv->ServerOpts.socketOpt.keepalive = 1;
	serv->ServerOpts.socketOpt.reuseaddr = 1;
	serv->Socket = socket(AF_INET, SOCK_STREAM, 0);
	// for sockets
	struct sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(S_PORT);
	// Do this to get IP of inter, hate it!
	/*struct ifreq ifr;
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, inter, IFNAMSIZ-1);
	ioctl(serv->Socket, SIOCGIFADDR, &ifr);
	// set fd as nonblocking
	fcntl(serv->Socket, F_SETFL, O_NONBLOCK, 1);*/

	sockaddr.sin_addr.s_addr = getInterIP(serv->Socket, inter);

	//serv->Clientlist.nClients = 0;
	serv->IP = sockaddr.sin_addr.s_addr; // src IP

	strcpy(serv->serverName, serverName);
	memcpy(serv->dir, Dir, strlen(Dir));
	
	setSockOpts(serv->Socket, &serv->ServerOpts.socketOpt, "\x01\x01\x00");

	if (bind(serv->Socket, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) == -1){
		perror("Server::Init::Error BIND");
		return NULL;
	}

	serv->size = sizeof(*serv) + strlen(serv->dir);
	serv->ServerOpts.sockaddr = (struct sockaddr*)&sockaddr;
	serv->ServerOpts.socklen = sizeof(sockaddr);

	strcpy(serv->client.name, serverName);
	if (strlen(ip) > 0){
		serv->client.Socket = connectToNetwork(ip, &serv->client);
		serv->destIP = inet_addr(ip);
	}

	serv->kqueueInstance = kqueue();
	serv->lkqueueInstance = kqueue();

	struct kevent ev;
	EV_SET(&ev, serv->Socket, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, "LISTEN");
	kevent(serv->lkqueueInstance, &ev, 1, NULL, 0, NULL);
	return serv;
}

int delClient(int fd, Server* serv){
	for (int i = 0; i < MAX_CLIENTS; i++){
		if (serv->Clientlist.clients[i].Socket == fd){
			serv->Clientlist.clients[i].Socket = 0;
			memset(serv->Clientlist.clients[i].name, 0, strlen(serv->Clientlist.clients[i].name));
			serv->Clientlist.clients[i].socketMode = 0;
			serv->nConn -= 1;
			printf("\nDeleted Client %i", fd);
			return 0;
		}
	} 
	return -1;
}

int addClient(int fd, Server* serv){
	struct kevent ev;

	if (serv->nConn >= MAX_CLIENTS){
		close(fd);
		printf("\nClient MAX Reached");
		return 0;
	}


	for(int i = 0; i <= MAX_CLIENTS; i++){
		if (i > MAX_CLIENTS-1){
			return -1;
		}

		else if (serv->Clientlist.clients[i].Socket == 0){
			serv->Clientlist.clients[i].Socket = fd;
			serv->nConn += 1;
			EV_SET(&ev, fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, NULL);
			kevent(serv->kqueueInstance, &ev, 1, NULL, 0, NULL);
			printf("\nAdded %i to kqueue", fd);
			return 0;
		}
	}

	return -1;
}

int checkSockets(Server* serv){
	struct timespec ts;
	ts.tv_sec = 1;
       	ts.tv_nsec = 5000000;
	int nevents = kevent(serv->kqueueInstance, NULL, 0, serv->Events, 10, &ts);
	printf("\n%i", nevents);
	if (nevents == 0){
		return 0;
	}

	for (int i = 0; i < MAX_EVENTS; i++){

		if (serv->Events[i].filter == EVFILT_READ && serv->Events[i].flags != EV_EOF){
			printf("\n%s", serv->Events[i].udata);
			printf("\nreading fd: %lu", serv->Events[i].ident);
			Packet* buf = (Packet*) malloc(sizeof(Packet));
			if (recv(serv->Events[i].ident, buf, sizeof(*buf), 0) == -1){
				perror("read Failed:");
				return errno;
			}

			struct in_addr addr;

			printf("\nProtocol: %s", buf->Proto);
			printf("\nDatalen: %i", buf->datalen);
			printf("\nMode: %x", *buf->Mode);
			addr.s_addr = buf->IP;
			printf("\nDst IP: %s", inet_ntoa(addr));
			read(serv->Events[i].ident, buf->data, buf->datalen);
			printf("\n%s", ((struct BROD*)buf->data)->fileReq);

			delClient(serv->Events[i].ident, serv);
			close(serv->Events[i].ident);

		
		}
	}

	return 0;
}


int ServerListen(Server* serv){
	struct kevent ev;
	struct timespec ts;
	ts.tv_sec = 1;
       	ts.tv_nsec = 0;

	int nSockets = kevent(serv->lkqueueInstance, NULL, 0, &ev, 1, &ts);
	printf("\nListening Events: %i", nSockets);

	if (ev.filter == EVFILT_READ && ev.ident == serv->Socket){
		int fd = accept(ev.ident, serv->ServerOpts.sockaddr, &serv->ServerOpts.socklen);	
		return fd;
	}
	return 0;

}
