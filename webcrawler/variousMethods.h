#ifndef _VARIOUSMETHODS_H_
#define _VARIOUSMETHODS_H_

#include "linksQueue.h"

void pickArgumentsMain(int argc,char* argv[],char** hostIP,int* port,int* commandPort,int* numThreads,char** saveDir,char** startingUrl);
void readGetLinesFromServer(linksQueue* queue, char* host, int socket, char* saveDir);
char* createFileSaveDir(char* saveDir, char* response, char* fileName);

#endif