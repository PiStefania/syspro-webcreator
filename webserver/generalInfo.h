#ifndef _GENERAL_INFO_H_
#define _GENERAL_INFO_H_

typedef struct generalInfo{
	int millis;				//started milliseconds
	long startTime;			//started seconds
	int pagesServed;		//pages sent
	long bytesServed;		//bytes sent
}generalInfo;

generalInfo* createGeneralInfo();
void printStats(generalInfo* info, int newsock);

#endif