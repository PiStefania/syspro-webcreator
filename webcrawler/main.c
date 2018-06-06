#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include "variousMethods.h"
#include "generalInfo.h"

int main (int argc,char* argv[]){
	//get time
	struct timeb before;
	ftime(&before);

	generalInfo* info = createGeneralInfo();
	info->startTime = before.time;
	info->millis = before.millitm;
	
	//check arguments
	int serverPort = 0, commandPort = 0, numThreads = 0;
	char* hostIP = NULL;
	char* saveDir = NULL;
	char* startingUrl = NULL;
	pickArgumentsMain(argc,argv,&hostIP,&serverPort,&commandPort,&numThreads,&saveDir,&startingUrl);
	
	if(serverPort == commandPort){
		printf("Server port == Command Port. Abort.\n");
		free(info);
		info = NULL;
		exit(1);
	}
	
	//create linksQueue
	linksQueue* queue = createLinksQueue();
	createdLinks* created = createCreatedLinks();
	
	//create threads
	threads* th = initializeThreads(numThreads,info,saveDir,startingUrl,hostIP,queue,created,serverPort);
	
	createManageSockets(serverPort, commandPort, th);
	
	destroyCreatedLinks(&created);
	destroyLinksQueue(&queue);
	free(info);
	info = NULL;
	
	destroyThreads(&th);
	
	return 0;
}