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
	int servingPort=0,commandPort=0,numThreads=0;
	char* rootDir = NULL;
	pickArgumentsMain(argc,argv,&servingPort,&commandPort,&numThreads,&rootDir);
	createManageSockets(servingPort, commandPort, rootDir, info);
	
	free(info);
	info = NULL;
	return 0;
}