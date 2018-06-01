#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 		// sockets
#include <sys/socket.h> 	// sockets
#include <netinet/in.h> 	// internet sockets
#include <unistd.h> 		// read, write, close
#include <netdb.h> 			// gethostbyaddr
#include "variousMethods.h"
#include "linksQueue.h"


int main (int argc,char* argv[]){
	
	//check arguments
	int serverPort = 0, commandPort = 0, numThreads = 0;
	char* hostIP = NULL;
	char* saveDir = NULL;
	char* startingUrl = NULL;
	pickArgumentsMain(argc,argv,&hostIP,&serverPort,&commandPort,&numThreads,&saveDir,&startingUrl);
	
	int port, sock, i;
	struct sockaddr_in server;
	struct sockaddr *serverptr = (struct sockaddr*)&server;
	struct hostent *rem;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){		// Create socket
		perror("socket");
		exit(1);
	}
	if ((rem = gethostbyname(hostIP)) == NULL){			// Find server address
		herror("gethostbyname"); 
		exit(1);
	}
	port = serverPort; 										// Convert port number to integer
	server.sin_family = AF_INET; 							// Internet domain 
	memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
	server.sin_port = htons(port); 							// Server port 
	if (connect(sock, serverptr, sizeof(server)) < 0){		// Initiate connection 
		perror("connect");
		exit(1);
	}
	
	//create linksQueue
	linksQueue* queue = createLinksQueue();
	char* link = convertToLink(startingUrl);
	createdLinks* created = createCreatedLinks();
	pushLinksQueue(queue,link,created);
	free(link);
	link = NULL;
	
	//read lines and send to server
	readGetLinesFromServer(queue,hostIP,sock,saveDir,created);
	
	destroyCreatedLinks(&created);
	destroyLinksQueue(&queue);
	close(sock);
	
	return 0;
}