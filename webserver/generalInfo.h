#ifndef _GENERAL_INFO_H_
#define _GENERAL_INFO_H_

typedef struct generalInfo{
	long startTime;
	int pagesServed;
	long bytesServed;
}generalInfo;

generalInfo* createGeneralInfo();
void printStats(generalInfo* info);

#endif