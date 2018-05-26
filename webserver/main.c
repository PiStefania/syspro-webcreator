#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> 		// sockets
#include <sys/types.h> 		// sockets
#include <sys/socket.h> 	// sockets
#include <netinet/in.h> 	// internet sockets
#include <netdb.h> 			// gethostbyaddr
#include <unistd.h>
#include "variousMethods.h"


int main (int argc,char* argv[]){
	
	//check arguments
	int servingPort=0,commandPort=0,numThreads=0;
	char* rootDir = NULL;
	pickArgumentsMain(argc,argv,&servingPort,&commandPort,&numThreads,&rootDir);
	
	//create socket for serving
	int port, sock, newsock;
	struct sockaddr_in server, client;
	socklen_t clientlen;
	struct sockaddr *serverptr=(struct sockaddr *)&server;
	struct sockaddr *clientptr=(struct sockaddr *)&client;
	struct hostent *rem;
	port = servingPort;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("socket");
	server.sin_family = AF_INET; 						// Internet domain
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port); 						// The given port
	if (bind(sock, serverptr, sizeof(server)) < 0){		// Bind socket to address
		perror("bind");
		exit(1);
	}
	if (listen(sock, 5) < 0){
		perror("listen");
		exit(1);
	}
	//printf("Listening for connections to port %d\n", port);		//Listen for connections
	while (1) { 
		clientlen = sizeof(client);
		if ((newsock = accept(sock, clientptr, &clientlen))< 0){ 	// accept connection
			perror("accept");
			exit(1);
		}
		if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL){		// Find client's name
			herror("gethostbyaddr"); 
			exit(1);
		}
		
		readGetLinesFromCrawler(newsock);
	}
	printf("Closing connection.\n");
	close(sock);
	close(newsock); 						// Close socket
	return 0;
}