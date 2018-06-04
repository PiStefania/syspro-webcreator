#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h> 		// sockets
#include <sys/types.h> 		// sockets
#include <sys/socket.h> 	// sockets
#include <netinet/in.h> 	// internet sockets
#include <netdb.h> 			// gethostbyaddr
#include <dirent.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h> 		//FD_SET, FD_ISSET, FD_ZERO macros
#include <errno.h>
#include "variousMethods.h"
#include "httpRequests.h"
#include "generalInfo.h"
#define DEF_BUFFER_SIZE 4096
#define LISTEN_SIZE 128


//specifies whether or not the arguments given are correct
void pickArgumentsMain(int argc,char* argv[],int* servingPort,int* commandPort,int* numThreads,char** rootDir){
		
	if(argc == 9){
		for(int i=0; i<argc; i++){
			if(strcmp(argv[i],"-p")==0){
				*servingPort=atoi(argv[i+1]);
			}if(strcmp(argv[i],"-c")==0){
				*commandPort=atoi(argv[i+1]);
			}if(strcmp(argv[i],"-t")==0){
				*numThreads=atoi(argv[i+1]);
			}if(strcmp(argv[i],"-d")==0){
				*rootDir=argv[i+1];
			}
		}
	}
	else{
		printf("Wrong number of arguments. Terminate process.\n");
		exit(1);
	}
	
	if(*servingPort == 0){
		printf("Serving Port is 0\n");
		exit(1);
	}
	
	if(*commandPort == 0){
		printf("Command Port is 0\n");
		exit(1);
	}
	
	if(*numThreads == 0){
		printf("Number of Threads is 0\n");
		exit(1);
	}
	
	if(*rootDir == NULL){
		printf("Root directory is NULL\n");
		exit(1);
	}
	
	DIR* dir = opendir(*rootDir);
	if (!dir){
    	printf("Root directory doesn't exist\n");
		closedir(dir);
		exit(1);
	}else{
		struct dirent* file = readdir(dir);
		if(file == NULL){
			printf("Root directory is empty\n");
			closedir(dir);
			exit(1);
		}
		closedir(dir);
	}
}

void readGetLinesFromCrawler(int newsock,char* rootDir,generalInfo* info){
	char ch;
	while(1){
		int chars = 0;
		if(read(newsock, &chars, sizeof(int)) < 0){
			perror("read");
			exit(1);
		}
		
		char buffer[chars+1];
		if(read(newsock, &buffer, chars) < 0){
			perror("read");
			exit(1);
		}
		buffer[chars] = '\0';
		//check request
		char* response = NULL;
		char* file = NULL;
		int checkRequest = checkRequestInfo(buffer,&file);
		if(file == NULL){
			printf("NO FILE GIVEN\n");
			break;
		}else if(!checkRequest){
			printf("WRONG INPUT\n");
			break;
		}
		else{	//get answer for request
			int fullLength = strlen(file) + strlen(rootDir) + 2;
			char* fullPath = malloc(fullLength*sizeof(char));
			strcpy(fullPath,rootDir);
			strcat(fullPath,file);
			//construct response for webcrawler
			response = constructResponse(fullPath);
			free(fullPath);
			fullPath = NULL;
		}
				
		if(response != NULL){
			info->pagesServed++;
			info->bytesServed += strlen(response);
			int lengthResponse = strlen(response);
			//initially write size of answer in socket
			if(write(newsock, &lengthResponse, sizeof(int)) < 0){
				perror("write size");
				exit(1);
			}
		
			int div = lengthResponse / DEF_BUFFER_SIZE;
			if(div>1){
				int bef = 0;
				for(int i=0;i<=div;i++){
					if(i==div){
						if(write(newsock, response + bef, lengthResponse-bef) < 0){
							perror("write last part");
							exit(1);
						}
					}else{
						if(write(newsock, response + bef, DEF_BUFFER_SIZE) < 0){
							perror("write part");
							exit(1);
						}
					}
					bef += DEF_BUFFER_SIZE;
				}
			}else{
				if(write(newsock, response, lengthResponse) < 0){
					perror("write");
					exit(1);
				}
			}
			free(file);
			file = NULL;
			free(response);
			response = NULL;
		}
	}
}


int getNumberLength(int no){
	int numberLength;
	if(no == 0){
		numberLength = 1;
	}else{
		numberLength = floor(log10(abs(no))) + 1;
	}
	
	return numberLength;
}

void createManageSockets(int servingPort, int commandPort, threads* th){
	//set of socket descriptors
    fd_set readfds;
	
	//create socket for serving
	int port, sock, newsock;
	struct sockaddr_in server, client;
	socklen_t clientlen;
	struct sockaddr *serverptr=(struct sockaddr *)&server;
	struct sockaddr *clientptr=(struct sockaddr *)&client;
	struct hostent *rem;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("socket");
	server.sin_family = AF_INET; 						// Internet domain
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(servingPort); 						// The given port
	if (bind(sock, serverptr, sizeof(server)) < 0){		// Bind socket to address
		perror("bind");
		exit(1);
	}
	if (listen(sock, LISTEN_SIZE) < 0){
		perror("listen");
		exit(1);
	}
	
	//create socket for command
	struct sockaddr_in serverCommand;
	struct sockaddr *serverCommandPtr=(struct sockaddr *)&serverCommand;
	int sockCommand;
	if ((sockCommand = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("socket command");
	serverCommand.sin_family = AF_INET; 						// Internet domain
	serverCommand.sin_addr.s_addr = htonl(INADDR_ANY);
	serverCommand.sin_port = htons(commandPort); 						// The given port
	if (bind(sockCommand, serverCommandPtr, sizeof(serverCommand)) < 0){		// Bind socket to address
		perror("bind");
		exit(1);
	}
	if (listen(sockCommand, LISTEN_SIZE) < 0){
		perror("listen");
		exit(1);
	}
	
	int maxSd;
	int whileFlag = 1;
	while (whileFlag) { 
		//clear the socket set
        FD_ZERO(&readfds);
		
		//add server socket to set
        FD_SET(sock, &readfds);
        FD_SET(sockCommand, &readfds);
		maxSd = sockCommand;
		
		//wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        int activity = select( maxSd + 1 , &readfds , NULL , NULL , NULL);
    
        if ((activity < 0) && (errno!=EINTR)) 
        {
            printf("select error");
        }
		
		 //If something happened on the master socket , then its an incoming connection
        if (FD_ISSET(sock, &readfds)) 
        {
            clientlen = sizeof(client);
			if ((newsock = accept(sock, clientptr, &clientlen))< 0){ 	// accept connection
				perror("accept");
				exit(1);
			}
			if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL){		// Find client's name
				herror("gethostbyaddr"); 
				exit(1);
			}
			
			//insert fd to poolData
			insertPoolData(th,newsock);
			pthread_cond_signal(&(th->notEmpty));
		}else if(FD_ISSET(sockCommand, &readfds)){
			clientlen = sizeof(client);
			if ((newsock = accept(sockCommand, clientptr, &clientlen)) < 0){ 	// accept connection
				perror("accept");
				exit(1);
			}
			whileFlag = readFromCommandPort(newsock,th);
		}
	}
	printf("Closing connections.\n");
	close(sock);
	close(sockCommand);
}

int readFromCommandPort(int newsock, threads* th){
	char ch;
	char buffer[10];
	int i=0;
	while(read(newsock, &ch, 1) > 0){
		if(ch == '\n')
			break;
		buffer[i] = ch;
		i++;
	}
	buffer[i-1] = '\0';
	if(strcmp(buffer,"STATS")==0){
		printStats(th->info,newsock);
	}else if(strcmp(buffer,"SHUTDOWN")==0){		
		return 0;
	}
	return 1;
}
