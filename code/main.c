#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/event.h>
#include <sys/time.h>
#include "client.h"
#include "server.h"
#include "core.h"

int main(int argc, char* argv[]){
	
	if (strcmp(argv[1], "client")==0){
		Client* test1 = NULL;
		test1 = Cl_Init(argv[2], argv[3]);
		printf("Connecting to %s", argv[4]);
		if (connectToNetwork(argv[4], test1) == -1){
			perror("Failed to connect to client");
		}

		makeFileReq(test1->Socket, test1->name, argv[5]);
		close(test1->Socket);
		free(test1);

		return 0;
	}

	if (strcmp(argv[1], "serv") == 0){
		Server* test2 = NULL;
		test2 = Init(argv[2], argv[3], argv[4], argv[5]);

		assert(test2 != NULL);

		assert(listen(test2->Socket, 10)==0);

		while (true){
			int fd = ServerListen(test2);

			if (fd > 0){
				addClient(fd, test2);
			}
			int fds[MAX_EVENTS];

			system("clear");
			checkSockets(test2, fds);
			SocketManager(fds, test2);
			IdManager(&test2->Traclist);
		}
		free(test2);
	}
	return 0;
}
