#ifndef _GENERAL_INFO_H_
#define _GENERAL_INFO_H_

typedef struct generalInfo{
	int millis;
	long startTime;
	int pagesDownloaded;
	long bytesDownloaded;
}generalInfo;

generalInfo* createGeneralInfo();
void printStats(generalInfo* info, int socket);
void executeJobExecutor(char* searchWords, char* docFile);

#endif