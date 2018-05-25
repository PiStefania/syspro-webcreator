#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "variousMethods.h"

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
	}

}