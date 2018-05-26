#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "variousMethods.h"
#define DEF_BUFFER_SIZE 128

//specifies whether or not the arguments given are correct
void pickArgumentsMain(int argc,char* argv[],char** hostIP,int* port,int* commandPort,int* numThreads,char** saveDir,char** startingUrl){
		
	if(argc == 12){
		for(int i=0; i<argc; i++){
			if(strcmp(argv[i],"-h")==0){
				*hostIP=argv[i+1];
				printf("hostIP: %s\n",*hostIP);
			}if(strcmp(argv[i],"-p")==0){
				*port=atoi(argv[i+1]);
				printf("port: %d\n",*port);
			}if(strcmp(argv[i],"-c")==0){
				*commandPort=atoi(argv[i+1]);
				printf("commandPort: %d\n",*commandPort);
			}if(strcmp(argv[i],"-t")==0){
				*numThreads=atoi(argv[i+1]);
				printf("numThreads: %d\n",*numThreads);
			}if(strcmp(argv[i],"-d")==0){
				*saveDir=argv[i+1];
				printf("saveDir: %s\n",*saveDir);
			}
		}
		*startingUrl = argv[argc-1];
		printf("startingUrl: %s\n",*startingUrl);
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
	}
	
	if(*startingUrl == NULL){
		printf("Starting Url is NULL\n");
		exit(1);
	}
}

void readGetLinesFromServer(int socket){
	int readStdin;
	size_t len = 0;
	char* line = NULL;
	char ch;
	char* buffer = malloc(DEF_BUFFER_SIZE*sizeof(char));
	int i=0;
	int div=1;
	while(1){
		if((readStdin = getline(&line, &len, stdin)) != -1){
			if(write(socket, line, strlen(line)) < 0){
				perror("write");
				exit(1);
			}
		}else{
			break;
		}
		
		while(read(socket, &ch, 1) > 0){
			//end of line reached
			if(ch == '\n'){
				i=0;
				div=1;
				break;
			}
			//max size reached
			if(i == (DEF_BUFFER_SIZE/div)){
				div++;
				buffer = realloc(buffer,(div*DEF_BUFFER_SIZE)*sizeof(char));
			}
			buffer[i] = ch;
			i++;
		}
		printf("SENT: %s\n",buffer);
		memset(buffer,0,strlen(buffer));
	}
	free(buffer);
	buffer = NULL;
}