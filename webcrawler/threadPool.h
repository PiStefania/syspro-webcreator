#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <pthread.h>
#include "generalInfo.h"

typedef struct threads threads;

#include "linksQueue.h"

struct threads{
	int noThreads;
	pthread_t* tids; 				// execution threads
	pthread_cond_t notEmpty;
	pthread_cond_t notFull;
	pthread_mutex_t lockData;
	pthread_mutex_t lockAdditional;
	linksQueue* queue;
	generalInfo* info;
	char* saveDir;
	char* startingUrl;
	char* hostIP;
	createdLinks* created;
	int servingPort;
};

//functions for pool data
linkNode* popFromQueue(threads* th);
void insertQueue(threads* th, char* link);

//functions for threads
threads* initializeThreads(int numThreads, generalInfo* info, char* saveDir, char* startingUrl, char* hostIP, linksQueue* queue, createdLinks* created, int servingPort);
void* connectHandler(void* args);
void destroyThreads(threads** th);

#endif