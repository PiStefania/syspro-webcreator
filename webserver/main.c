#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include "variousMethods.h"
#include "generalInfo.h"
#include "threadPool.h"


int main (int argc,char* argv[]){
	//get time
	struct timeb before;
	ftime(&before);

	generalInfo* info = createGeneralInfo();
	info->startTime = before.time;
	info->millis = before.millitm;
	
	
	//check arguments
	int servingPort=0,commandPort=0,numThreads=0;
	char* rootDir = NULL;
	pickArgumentsMain(argc,argv,&servingPort,&commandPort,&numThreads,&rootDir);
	
	if(servingPort == commandPort){
		printf("Serving and command port are the same. Abort.\n");
		free(info);
		info = NULL;
		exit(1);
	}
	
	//create threads
	threads* th = initializeThreads(numThreads,info,rootDir);
	
	//manage sockets
	createManageSockets(servingPort, commandPort, th);
	
	free(info);
	info = NULL;
	
	//destroyThreads(&th);
	return 0;
}