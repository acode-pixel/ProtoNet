#include "serverModules/tracModule.h"

int addTracItem(tracList* Traclist, uint tracID, char* fileRequester, uint8_t hops, uint8_t lifetime, void* fileOffset, char* fileReq){
	for (int i = 0; i < MAX_CLIENTS; i++){
		if (Traclist->tracs[i].tracID != 0){
			continue;
		}

		fillTracItem(&Traclist->tracs[i], tracID, fileRequester, hops, lifetime, fileOffset, fileReq);
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
	addTracItem(&serv->Traclist, trac->tracID, buf->Name, trac->hops, trac->lifetime, NULL, ((struct BROD*)buf->data)->fileReq);

	return 0;
}

int IdManager(tracList* traclist){
	printf("tracID|hops|lifetime|fileOffset|confirmed|File Request\n");
	printf("========================================================================\n");

	for (int i = 0; i < MAX_CLIENTS; i++){
		if(traclist->tracs[i].tracID != 0){
			printf("%i|", traclist->tracs[i].tracID);
			printf("%i|", traclist->tracs[i].hops);
			printf("%i|", traclist->tracs[i].lifetime);
			printf("%p|", traclist->tracs[i].fileOffset);
			printf("%x|", traclist->tracs[i].confirmed);
			printf("%s\n", traclist->tracs[i].fileReq);
		}
	}
	return 0;
}