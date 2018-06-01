#ifndef _VARIOUSMETHODS_H_
#define _VARIOUSMETHODS_H_

#include "generalInfo.h"

void pickArgumentsMain(int argc,char* argv[],int* servingPort,int* commandPort,int* numThreads,char** rootDir);
void readGetLinesFromCrawler(int newsock,char* rootDir,generalInfo* info);
int getNumberLength(int no);
void createManageSockets(int servingPort, int commandPort, char* rootDir, generalInfo* info);
int readFromCommandPort(int newsock, generalInfo* info);

#endif