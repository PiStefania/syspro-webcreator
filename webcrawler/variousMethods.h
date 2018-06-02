#ifndef _VARIOUSMETHODS_H_
#define _VARIOUSMETHODS_H_

#include "linksQueue.h"
#include "generalInfo.h"

void pickArgumentsMain(int argc,char* argv[],char** hostIP,int* port,int* commandPort,int* numThreads,char** saveDir,char** startingUrl);
void readGetLinesFromServer(linksQueue* queue, char* host, int socket, char* saveDir, createdLinks* created, generalInfo* info);
char* createFileSaveDir(char* saveDir, char* response, char* fileName);
void createManageSockets(int servingPort, int commandPort, char* hostIP, char* startingUrl, char* saveDir, generalInfo* info);
int readFromCommandPort(int socket, generalInfo* info, char* saveDir);

#endif