#ifndef _VARIOUSMETHODS_H_
#define _VARIOUSMETHODS_H_

#include "generalInfo.h"
#include "threadPool.h"

void pickArgumentsMain(int argc,char* argv[],int* servingPort,int* commandPort,int* numThreads,char** rootDir);
void readGetLinesFromCrawler(int newsock, threads* th);
int getNumberLength(int no);
void createManageSockets(int servingPort, int commandPort, threads* th);
int readFromCommandPort(int newsock, threads* th);

#endif