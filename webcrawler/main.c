#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 		// sockets
#include <sys/socket.h> 	// sockets
#include <netinet/in.h> 	// internet sockets
#include <unistd.h> 		// read, write, close
#include <netdb.h> 			// gethostbyaddr
#include "variousMethods.h"


int main (int argc,char* argv[]){
	
	//check arguments
	int serverPort = 0, commandPort = 0, numThreads = 0;
	char* hostIP = NULL;
	char* saveDir = NULL;
	char* startingUrl = NULL;
	pickArgumentsMain(argc,argv,&hostIP,&serverPort,&commandPort,&numThreads,&saveDir,&startingUrl);
	
	int port, sock, i;
	char buf[256];
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
	printf("Connecting to %s port %d\n", argv[1], port);
	do {
		printf("Give input string: ");
		fgets(buf, sizeof(buf), stdin); 					// Read from stdin
		for(i=0; buf[i] != '\0'; i++) { 					// For every char 
			if (write(sock, buf + i, 1) < 0){				// Send i-th character
				perror("write");
				exit(1);
			}if (read(sock, buf + i, 1) < 0){				// receive i-th character transformed
				perror("read");
				exit(1);
			}
		}
		printf("Received string: %s", buf);
	}while(strcmp(buf, "END\n") != 0); 						// Finish on "end"
	close(sock);
	return 0;
}