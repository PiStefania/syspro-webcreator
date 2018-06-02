#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "generalInfo.h"

generalInfo* createGeneralInfo(){
	generalInfo* info = malloc(sizeof(generalInfo));
	info->millis = 0;
	info->startTime = 0;
	info->pagesDownloaded = 0;
	info->bytesDownloaded = 0;
	return info;
}

void printStats(generalInfo* info, int socket){
	//get duration
	struct timeb after;
	ftime(&after);
	long secs = (double)(after.time - info->startTime);
	int hours = secs / (60 * 60);
	int remainder = secs % 3600;
	long minutes = remainder / 60;
	long seconds = remainder % 60;
	
	char* buffer = malloc(100*sizeof(char));
	int millis = abs(after.millitm - info->millis);
	
	if(minutes > 10 && seconds > 10){
		sprintf(buffer,"Crawler up for 0%d:%ld:%ld.%d, downloaded %d pages, %ld bytes\n",hours,minutes,seconds,millis,info->pagesDownloaded,info->bytesDownloaded);
		if(write(socket,buffer,strlen(buffer)) < 0){
			perror("write command");
			free(buffer);
			buffer = NULL;
		}
	}else if(minutes > 10 && seconds < 10){
		sprintf(buffer,"Crawler up for 0%d:%ld:0%ld.%d, downloaded %d pages, %ld bytes\n",hours,minutes,seconds,millis,info->pagesDownloaded,info->bytesDownloaded);
		if(write(socket,buffer,strlen(buffer)) < 0){
			perror("write command");
			free(buffer);
			buffer = NULL;
		}
	}else if(minutes < 10 && seconds > 10){
		sprintf(buffer,"Crawler up for 0%d:0%ld:%ld.%d, downloaded %d pages, %ld bytes\n",hours,minutes,seconds,millis,info->pagesDownloaded,info->bytesDownloaded);
		if(write(socket,buffer,strlen(buffer))  < 0){
			perror("write command");
			free(buffer);
			buffer = NULL;
		}
	}else if(minutes < 10 && seconds < 10){
		sprintf(buffer,"Crawler up for 0%d:0%ld:0%ld.%d, downloaded %d pages, %ld bytes\n",hours,minutes,seconds,millis,info->pagesDownloaded,info->bytesDownloaded);
		if(write(socket,buffer,strlen(buffer)) < 0){
			perror("write command");
			free(buffer);
			buffer = NULL;
		}
	}
	free(buffer);
	buffer = NULL;
}

void executeJobExecutor(char* searchWords, char* docFile){
	printf("searchWords: '%s'\n",searchWords);
	char** words = malloc(10*sizeof(char*));
	for(int j=0;j<10;j++){
		words[j] = NULL;
	}
	char* token = strtok(searchWords," ");
	int i=1;
	while(i<10){
		words[i-1] = malloc((strlen(token)+2)*sizeof(char));
		strcpy(words[i-1],token);
		printf("words[%d]: '%s'\n",i-1,words[i-1]);
		token = strtok(NULL," ");
		i++;
	}
	
	//call jobExecutor
	char *args[]={"../jobExecutor/jobExecutor","-d",docFile,"-w","10"};
    execvp(args[0],args);
	
}