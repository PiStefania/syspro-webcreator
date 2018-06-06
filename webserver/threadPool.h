#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <pthread.h>
#include "generalInfo.h"

typedef struct poolData{
	int* bufferFds;			//buffer of fds
	int end;				//end of buffer
	int position;			//current size of buffer
	int start;
}poolData;

typedef struct threads{
	int noThreads;					//number of threads
	pthread_t* tids; 				// execution threads
	pthread_cond_t notEmpty;		//cond var for checking if poolData is empty
	pthread_cond_t notFull;			//cond var for checking if poolData is full
	pthread_mutex_t lockData;		//mutex for locking poolData
	pthread_mutex_t lockAdditional;		//mutex for locking additional data like generalInfo alterations
	poolData* data;						//data that the threads alter
	generalInfo* info;					//helpful for argument
	char* rootDir;						//helpful for argument
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