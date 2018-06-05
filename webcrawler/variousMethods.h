#ifndef _VARIOUSMETHODS_H_
#define _VARIOUSMETHODS_H_

#include "linksQueue.h"
#include "generalInfo.h"
#include "threadPool.h"

void pickArgumentsMain(int argc,char* argv[],char** hostIP,int* port,int* commandPort,int* numThreads,char** saveDir,char** startingUrl);
void readGetLinesFromServer(int socket, threads* th);
char* createFileSaveDir(char* saveDir, char* response, char* fileName);
void createManageSockets(int servingPort, int commandPort, threads* th);
int readFromCommandPort(int socket, generalInfo* info, char* saveDir);

#endif