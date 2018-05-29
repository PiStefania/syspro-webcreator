#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "variousMethods.h"
#define DEF_BUFFER_SIZE 4096

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
	int chars=0;
	char buffer[DEF_BUFFER_SIZE];
	int requestFlag = 1;
	int init = 0;
	while(1){
		if((readStdin = getline(&line, &len, stdin)) != -1 && requestFlag){
			chars += strlen(line);
			if(chars > DEF_BUFFER_SIZE){
				break;
			}else if(strcmp(line,"\n")==0){
				if(write(socket, &chars, sizeof(int)) < 0){
					perror("write");
					exit(1);
				}

				if(write(socket, buffer, chars) < 0){
					perror("write");
					exit(1);
				}
				memset(buffer,0,chars);
				chars = 0;
				requestFlag = 0;
			}else{
				if(init==0){
					strcpy(buffer,line);
					init++;
				}else{
					strcat(buffer,line);
				}
			}
		}
		
		if(!requestFlag){
			int responseChars = 0;
			if(read(socket, &responseChars, sizeof(int)) < 0){
				perror("read");
				exit(1);
			}
			
			int div = responseChars / DEF_BUFFER_SIZE;
			char responseBuffer[responseChars];
			if(div>1){
				char temp[DEF_BUFFER_SIZE];
				for(int i=0;i<=div;i++){
					if(read(socket, temp, DEF_BUFFER_SIZE) < 0){
						perror("read");
						exit(1);
					}
					if(i==0){
						strcpy(responseBuffer,temp);
					}else{
						strcat(responseBuffer,temp);
					}
				}
				responseBuffer[responseChars-1] = '\0';
			}else{
				if(read(socket, responseBuffer, responseChars) < 0){
					perror("read");
					exit(1);
				}
			}
			printf("SENT: %s\n",responseBuffer);
			//if answer is positive, create file and add source
			memset(responseBuffer,0,responseChars);
			requestFlag = 1;
		}
	}
}