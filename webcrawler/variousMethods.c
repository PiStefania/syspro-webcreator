#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "variousMethods.h"
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

void readGetLinesFromServer(linksQueue* queue, char* host, int socket, char* saveDir, createdLinks* created){
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
			int responseChars = 0;
			if(read(socket, &responseChars, sizeof(int)) < 0){
				perror("read");
				exit(1);
			}

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
