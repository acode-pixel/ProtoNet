#include "client.h"

Client* Cl_Init(char* inter, char name[]){
	Client* cli = (Client*) malloc(sizeof(Client));
	cli->Socket = socket(AF_INET, SOCK_STREAM, 0);
	struct in_addr addr;
	addr.s_addr = getInterIP(cli->Socket, inter);
	if (name == NULL){
		strcpy(cli->name, (char*)(inet_ntoa(addr)));
	} 
	
	else {strcpy(cli->name, name);}

	return cli;
}

int connectToNetwork(char* IP, Client* cli){
	int tcpSocket = (cli->Socket == 0) ? socket(AF_INET, SOCK_STREAM, 0) : cli->Socket;

	struct sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(C_PORT);
	inet_aton(IP, &sockaddr.sin_addr);

	//printf("%i %p %lu", tcpSocket, &sockaddr, sizeof(sockaddr));
	
	if (connect(tcpSocket, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) == -1){
		return -1;
	}
	
	return tcpSocket;

}

int makeFileReq(int fd, char* IP, char File[]){
	struct BROD* br = (struct BROD*)malloc(sizeof(struct BROD) + strlen(File)+1);
	br->hops = 0x01;
	strcpy(br->fileReq, File);
	assert(sendPck(fd, inet_addr(IP), 1, br) == 0);
	free(br);
	return 0;

};
