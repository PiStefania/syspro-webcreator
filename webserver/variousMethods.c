#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <math.h>
#include "variousMethods.h"
#include "httpRequests.h"
#define DEF_BUFFER_SIZE 4096

//specifies whether or not the arguments given are correct
void pickArgumentsMain(int argc,char* argv[],int* servingPort,int* commandPort,int* numThreads,char** rootDir){
		
	if(argc == 9){
		for(int i=0; i<argc; i++){
			if(strcmp(argv[i],"-p")==0){
				*servingPort=atoi(argv[i+1]);
				printf("servingPort: %d\n",*servingPort);
			}if(strcmp(argv[i],"-c")==0){
				*commandPort=atoi(argv[i+1]);
				printf("commandPort: %d\n",*commandPort);
			}if(strcmp(argv[i],"-t")==0){
				*numThreads=atoi(argv[i+1]);
				printf("numThreads: %d\n",*numThreads);
			}if(strcmp(argv[i],"-d")==0){
				*rootDir=argv[i+1];
				printf("rootDir: %s\n",*rootDir);
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

void readGetLinesFromCrawler(int newsock,char* rootDir){
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
		}else if(!checkRequest){
			printf("WRONG INPUT\n");
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


int getNumberLength(int no){
	int numberLength;
	if(no == 0){
		numberLength = 1;
	}else{
		numberLength = floor(log10(abs(no))) + 1;
	}
	
	return numberLength;
}