#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <pthread.h>
#include "generalInfo.h"

typedef struct poolData{
	int* bufferFds;
	int end;
	int position;
}poolData;

typedef struct threads{
	int noThreads;
	pthread_t* tids; 				// execution threads
	pthread_cond_t notEmpty;
	pthread_cond_t notFull;
	pthread_mutex_t lockData;
	pthread_mutex_t lockAdditional;
	poolData* data;
	generalInfo* info;
	char* rootDir;
}threads;

//functions for pool data
poolData* initializePoolData();
void insertPoolData(threads* th, int fd);
int getPoolData(threads* th);
void destroyPoolData(poolData** data);
void printFDs(poolData* data);

//functions for threads
threads* initializeThreads(int numThreads, generalInfo* info, char* rootDir);
void* acceptHandler(void* args);
void destroyThreads(threads** th);

#endif