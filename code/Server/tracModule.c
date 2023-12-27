#include "serverModules/tracModule.h"

int addTracItem(tracList* Traclist, uint tracID, uint8_t hops, uint8_t lifetime, void* fileOffset, char* fileReq){
	for (int i = 0; i < MAX_CLIENTS; i++){
		if (Traclist->tracs[i].tracID != 0){
			continue;
		}

		
		Traclist->tracs[i].tracID = tracID;
		Traclist->tracs[i].hops = hops;
		Traclist->tracs[i].lifetime = lifetime;
		Traclist->tracs[i].fileOffset = fileOffset;
		strcpy(Traclist->tracs[i].fileReq, fileReq);
		return 0;
	}

	printf("Maximum transactions reached\n");
	return -1;
}

int tracSpread(clientList* Clientlist, Packet* buf, Server* serv){
	struct TRAC* trac = (struct TRAC*) malloc(sizeof(struct TRAC));
	trac->hops = ((struct BROD*)buf->data)->hops;
	trac->lifetime = 255-trac->hops;
	srand(time(NULL));
	trac->tracID = rand();

	for (int i = 0; i < MAX_CLIENTS; i++){
		if (Clientlist->clients[i].Socket == 0){
			continue;
		}

		sendPck(Clientlist->clients[i].Socket, serv->serverName, SPTP_TRAC, trac);

	}

	free(trac);
	addTracItem(&serv->Traclist, trac->tracID, trac->hops, trac->lifetime, NULL, ((struct BROD*)buf->data)->fileReq);

	return 0;
}