#ifndef _GENERAL_INFO_H_
#define _GENERAL_INFO_H_

typedef struct generalInfo{
	int millis;					//started milliseconds
	long startTime;				//started seconds
	int pagesDownloaded;		//pages downloaded
	long bytesDownloaded;		//bytes downloaded
}generalInfo;

generalInfo* createGeneralInfo();
void printStats(generalInfo* info, int socket);
void executeJobExecutor(char* searchWords, char* saveDir, int socket);

#endif