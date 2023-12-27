#include "serverModules/parserModules.h"

int brodParser(Packet* buf, Client* client, Server* serv){
	char* fileReq = ((struct BROD*)buf->data)->fileReq;
	char filepath[strlen(serv->dir)+strlen(fileReq)];
	strcpy(filepath, serv->dir);
	strcat(filepath, fileReq);

	if(access(filepath, R_OK) == -1){
		char* data = "NO_FILE";
		sendPck(client->Socket, buf->Name, 1, data);
		return 0;
	} else {
		client->socketMode = SPTP_BROD;
		tracSpread(&serv->Clientlist, buf, serv);
	}
	return 0;
}