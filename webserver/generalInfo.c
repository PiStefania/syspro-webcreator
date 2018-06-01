#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "generalInfo.h"

generalInfo* createGeneralInfo(){
	generalInfo* info = malloc(sizeof(generalInfo));
	info->startTime = 0;
	info->pagesServed = 0;
	info->bytesServed = 0;
	return info;
}

void printStats(generalInfo* info){
	//get duration
	struct timespec after;
	clock_gettime(CLOCK_REALTIME, &after);
	long seconds = (double)(after.tv_sec - info->startTime);
	long hours = seconds / (60 * 60);
	long minutes = seconds / 60;
	
	if(minutes > 10 && seconds > 10){
		printf("Server up for 0%ld:%ld:%ld.%ld, served %d pages, %ld bytes\n",hours,minutes,seconds,seconds%1000,info->pagesServed,info->bytesServed);
	}else if(minutes > 10 && seconds < 10){
		printf("Server up for 0%ld:%ld:0%ld.%ld, served %d pages, %ld bytes\n",hours,minutes,seconds,seconds%1000,info->pagesServed,info->bytesServed);
	}else if(minutes < 10 && seconds > 10){
		printf("Server up for 0%ld:0%ld:%ld.%ld, served %d pages, %ld bytes\n",hours,minutes,seconds,seconds%1000,info->pagesServed,info->bytesServed);
	}else if(minutes < 10 && seconds < 10){
		printf("Server up for 0%ld:0%ld:0%ld.%ld, served %d pages, %ld bytes\n",hours,minutes,seconds,seconds%1000,info->pagesServed,info->bytesServed);
	}
}