#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h> 		// sockets
#include <sys/socket.h> 	// sockets
#include <netinet/in.h> 	// internet sockets
#include <unistd.h> 		// read, write, close
#include <netdb.h> 			// gethostbyaddr
#include <sys/time.h> 		//FD_SET, FD_ISSET, FD_ZERO macros
#include <errno.h>
#include "variousMethods.h"
#include "linksQueue.h"
#define DEF_BUFFER_SIZE 4096

//specifies whether or not the arguments given are correct
void pickArgumentsMain(int argc,char* argv[],char** hostIP,int* port,int* commandPort,int* numThreads,char** saveDir,char** startingUrl){
		
	if(argc == 12){
		for(int i=0; i<argc; i++){
			if(strcmp(argv[i],"-h")==0){
				*hostIP=argv[i+1];
			}if(strcmp(argv[i],"-p")==0){
				*port=atoi(argv[i+1]);
			}if(strcmp(argv[i],"-c")==0){
				*commandPort=atoi(argv[i+1]);
			}if(strcmp(argv[i],"-t")==0){
				*numThreads=atoi(argv[i+1]);
			}if(strcmp(argv[i],"-d")==0){
				*saveDir=argv[i+1];
			}
		}
		*startingUrl = argv[argc-1];
	}
	else{
		printf("Wrong number of arguments. Terminate process.\n");
		exit(1);
	}
	
	if(*hostIP == NULL){
		printf("Host or IP is NULL\n");
		exit(1);
	}
	
	if(*port == 0){
		printf("Port is 0\n");
		exit(1);
	}
	
	if(*commandPort == 0){
		printf("Comand Port is 0\n");
		exit(1);
	}
	
	if(*numThreads == 0){
		printf("Number of Threads is 0\n");
		exit(1);
	}
	
	if(*saveDir == NULL){
		printf("Save Directory is NULL\n");
		exit(1);
	}
	
	DIR* dir = opendir(*saveDir);
	if (!dir){
    	printf("Save directory doesn't exist\n");
		closedir(dir);
		exit(1);
	}else{
		char* command = malloc((strlen("rm -rf ")+strlen(*saveDir)+3)*sizeof(char));
		sprintf(command,"rm -rf %s/*",*saveDir);
		system(command);
		free(command);
		command = NULL;
		closedir(dir);
	}
	
	if(*startingUrl == NULL){
		printf("Starting Url is NULL\n");
		exit(1);
	}
}

void readGetLinesFromServer(linksQueue* queue, char* host, int socket, char* saveDir, createdLinks* created, generalInfo* info){
	int chars=0;
	int requestFlag = 1;
	int whileFlag = 1;
	while(whileFlag){
		char* fileName = NULL;
		if(!isEmptyLinksQueue(queue) && requestFlag){
			linkNode* node = popLinksQueue(queue);
			char* request = createRequest(node->link, host);
			fileName = malloc((strlen(node->link)+1)*sizeof(char));
			strcpy(fileName,node->link);
			destroyLinkNode(&node);
			chars = strlen(request);
			if(write(socket, &chars, sizeof(int)) < 0){
				perror("write size of lines");
				exit(1);
			}
			if(write(socket, request, chars) < 0){
				perror("write lines");
				exit(1);
			}
			requestFlag = 0;
			free(request);
			request = NULL;
		}
		else if(isEmptyLinksQueue(queue)){
			whileFlag = 0;
		}

		if(!requestFlag){
			info->pagesDownloaded++;
			int responseChars = 0;
			if(read(socket, &responseChars, sizeof(int)) < 0){
				perror("read");
				exit(1);
			}

			info->bytesDownloaded += responseChars;
			int div = responseChars / DEF_BUFFER_SIZE;
			char* responseBuffer = malloc((responseChars+1)*sizeof(char));
			if(div>1){
				int size = 0;
				char temp[DEF_BUFFER_SIZE];
				for(int i=0;i<=div;i++){
					if(i!=div){
						if(read(socket, temp, DEF_BUFFER_SIZE) < 0){
							perror("read");
							exit(1);
						}
						temp[DEF_BUFFER_SIZE] = '\0';
						if(i==0){
							strcpy(responseBuffer,temp);
						}else{
							strcat(responseBuffer,temp);
						}
						size += DEF_BUFFER_SIZE;
					}else{
						char temp1[responseChars-size+1];
						if(read(socket, temp1, responseChars-size) < 0){
							perror("read");
							exit(1);
						}
						temp1[responseChars-size] = '\0';
						strcat(responseBuffer,temp1);
					}
				}
			}else{
				if(read(socket, responseBuffer, responseChars) < 0){
					perror("read");
					exit(1);
				}
			}

			//insert content to new file
			char* content = createFileSaveDir(saveDir,responseBuffer,fileName);
			//insert links from content to queue
			char* site = strtok(fileName,"/");
			insertLinksQueueContent(queue,content,site,created);
			requestFlag = 1;
			free(responseBuffer);
			responseBuffer = NULL;
			if(fileName != NULL){
				free(fileName);
				fileName = NULL;
			}
		}
	}
}

char* createFileSaveDir(char* saveDir, char* response, char* fileName){
	char* content = strstr(response,"<!DOCTYPE");
	if(content == NULL){
		content = strstr(response,"<html>");
	}
	int lengthFile = strlen(saveDir) + strlen(fileName) + 2;
	char* file = malloc(lengthFile*sizeof(char));
	sprintf(file,"%s%s",saveDir,fileName);
	char* site = strtok(fileName,"/");
	char* fullPathDir = malloc((strlen(saveDir)+strlen(site)+2)*sizeof(char));
	sprintf(fullPathDir,"%s/%s",saveDir,site);
	int result = mkdir(fullPathDir, 0777);
	free(fullPathDir);
	fullPathDir = NULL;
	FILE* fp = fopen(file,"w");
	if(fp != NULL){
		fputs(content, fp);
        fclose(fp);
	}
	free(file);
	file = NULL;
	return content;
}

void createManageSockets(int servingPort, int commandPort, char* hostIP, char* startingUrl, char* saveDir, generalInfo* info){
	//create socket and connect with server
	int sock, i;
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
	server.sin_family = AF_INET; 							// Internet domain 
	memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
	server.sin_port = htons(servingPort); 							// Server port 
	
	
	//create linksQueue
	linksQueue* queue = createLinksQueue();
	char* link = convertToLink(startingUrl);
	createdLinks* created = createCreatedLinks();
	pushLinksQueue(queue,link,created);
	free(link);
	link = NULL;
	
	if (connect(sock, serverptr, sizeof(server)) < 0){		// Initiate connection 
		perror("connect");
		exit(1);
	}
	
	//read lines and send to server
	readGetLinesFromServer(queue,hostIP,sock,saveDir,created,info);
	
	//create commands socket
	socklen_t clientlen;
	struct sockaddr_in client;
	struct sockaddr *clientptr=(struct sockaddr *)&client;
	struct sockaddr_in crawlerCommand;
	struct sockaddr *crawlerCommandPtr=(struct sockaddr *)&crawlerCommand;
	int sockCommand;
	if ((sockCommand = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("socket command");
	crawlerCommand.sin_family = AF_INET; 						// Internet domain
	crawlerCommand.sin_addr.s_addr = htonl(INADDR_ANY);
	crawlerCommand.sin_port = htons(commandPort); 						// The given port
	if (bind(sockCommand, crawlerCommandPtr, sizeof(crawlerCommand)) < 0){		// Bind socket to address
		perror("bind");
		exit(1);
	}
	if (listen(sockCommand, 5) < 0){
		perror("listen");
		exit(1);
	}
	
	int maxSd;
	int whileFlag = 1;
	int newsock;
			
	//set of socket descriptors
    fd_set readfds;
	
	//clear the socket set
	FD_ZERO(&readfds);

	//add server socket to set
	FD_SET(sockCommand, &readfds);
	maxSd = sockCommand;
	while (whileFlag) { 
		//wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        int activity = select( maxSd + 1 , &readfds , NULL , NULL , NULL);
    
        if ((activity < 0) && (errno!=EINTR)) 
        {
            printf("select error");
        }
		
       if(FD_ISSET(sockCommand, &readfds)){
			clientlen = sizeof(client);
			if ((newsock = accept(sockCommand, clientptr, &clientlen)) < 0){ 	// accept connection
				perror("connect");
				exit(1);
			}
			whileFlag = readFromCommandPort(newsock, info, saveDir);
		}
	}
	printf("Closing connections.\n");
	close(sockCommand);
	close(newsock);
	destroyCreatedLinks(&created);
	destroyLinksQueue(&queue);
}

int readFromCommandPort(int socket, generalInfo* info, char* saveDir){
	char ch;
	char buffer[10];
	int i=0;
	while(read(socket, &ch, 1) > 0){
		if(ch == '\n')
			break;
		buffer[i] = ch;
		i++;
	}
	buffer[i-1] = '\0';
	if(strcmp(buffer,"STATS")==0){
		printStats(info,socket);
	}else if(strcmp(buffer,"SHUTDOWN")==0){
		return 0;
	}else{
		char* token = strtok(buffer," ");
		if(strcmp(token,"SEARCH")==0){
			token = strtok(NULL,"");
			executeJobExecutor(token, saveDir);
		}
	}
	return 1;
}
