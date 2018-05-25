#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "variousMethods.h"

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