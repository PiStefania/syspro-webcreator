#ifndef _VARIOUSMETHODS_H_
#define _VARIOUSMETHODS_H_

void pickArgumentsMain(int argc,char* argv[],char** hostIP,int* port,int* commandPort,int* numThreads,char** saveDir,char** startingUrl);
void readGetLinesFromServer(int socket);

#endif