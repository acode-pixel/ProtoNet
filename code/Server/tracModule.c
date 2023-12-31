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

tracItem* getTracItem(tracList* Traclist, char* name, uint tracID){
	if (name != NULL){
		for(int i = 0; i < MAX_CLIENTS; i++){
			if(strcmp(Traclist->tracs[i].fileRequester, name) == 0){
				return &Traclist->tracs[i];
			}
		}
	} else if (tracID != 0){
		for(int i = 0; i < MAX_CLIENTS; i++){
			if(tracID == Traclist->tracs[i].tracID){
				return &Traclist->tracs[i];
			}
		}
	}

	return (tracItem*)NULL;
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
	printf("tracID|deleted|client|socket|hops|lifetime|fileOffset|confirmed|canDelete|File Request\n");
	printf("========================================================================\n");

	for (int i = 0; i < MAX_CLIENTS; i++){
		if(traclist->tracs[i].tracID != 0){
			printf("%i|", traclist->tracs[i].tracID);
			printf("%x|", traclist->tracs[i].deleted);
			printf("%s|", traclist->tracs[i].fileRequester);
			printf("%i|", traclist->tracs[i].Socket);
			printf("%i|", traclist->tracs[i].hops);
			printf("%i|", traclist->tracs[i].lifetime);
			printf("%p|", traclist->tracs[i].fileOffset);
			printf("%x|", traclist->tracs[i].confirmed);
			printf("%x|", traclist->tracs[i].canDelete);
			printf("%s\n", traclist->tracs[i].fileReq);
		}
	}
	return 0;
}