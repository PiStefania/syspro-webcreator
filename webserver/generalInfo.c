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
	info->pagesServed = 0;
	info->bytesServed = 0;
	return info;
}

void printStats(generalInfo* info, int newsock){
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
		sprintf(buffer,"Server up for 0%d:%ld:%ld.%d, served %d pages, %ld bytes\n",hours,minutes,seconds,millis,info->pagesServed,info->bytesServed);
		if(write(newsock,buffer,strlen(buffer)) < 0){
			perror("write command");
			free(buffer);
			buffer = NULL;
		}
	}else if(minutes > 10 && seconds < 10){
		sprintf(buffer,"Server up for 0%d:%ld:0%ld.%d, served %d pages, %ld bytes\n",hours,minutes,seconds,millis,info->pagesServed,info->bytesServed);
		if(write(newsock,buffer,strlen(buffer)) < 0){
			perror("write command");
			free(buffer);
			buffer = NULL;
		}
	}else if(minutes < 10 && seconds > 10){
		sprintf(buffer,"Server up for 0%d:0%ld:%ld.%d, served %d pages, %ld bytes\n",hours,minutes,seconds,millis,info->pagesServed,info->bytesServed);
		if(write(newsock,buffer,strlen(buffer))  < 0){
			perror("write command");
			free(buffer);
			buffer = NULL;
		}
	}else if(minutes < 10 && seconds < 10){
		sprintf(buffer,"Server up for 0%d:0%ld:0%ld.%d, served %d pages, %ld bytes\n",hours,minutes,seconds,millis,info->pagesServed,info->bytesServed);
		if(write(newsock,buffer,strlen(buffer)) < 0){
			perror("write command");
			free(buffer);
			buffer = NULL;
		}
	}
	free(buffer);
	buffer = NULL;
}