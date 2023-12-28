#include "serverModules/parserModules.h"

int brodParser(Packet* buf, Client* client, Server* serv){

    struct stat st;
    char* data;

    if (strcmp((char*)buf->data, "LEAVE") == 0){
        data = "OK";
        sendPck(client->Socket, serv->serverName, SPTP_BROD, data);
        printf("Deleting client");
        close(client->Socket);
		delClient(client->Socket, serv);
        return 0;
    }

	char* fileReq = ((struct BROD*)buf->data)->fileReq;
	char filepath[strlen(serv->dir)+strlen(fileReq)];
	strcpy(filepath, serv->dir);
	strcat(filepath, fileReq);
    stat(filepath, &st);

	if(access(filepath, R_OK) == -1 || st.st_size > 1024000000){
		data = "NO_FILE";
		sendPck(client->Socket, buf->Name, 1, data);
	} else {
		client->socketMode = SPTP_BROD;
		tracSpread(&serv->Clientlist, buf, serv);
	}
	return 0;
}