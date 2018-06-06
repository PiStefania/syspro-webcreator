#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <pthread.h>
#include "generalInfo.h"

typedef struct threads threads;

#include "linksQueue.h"

struct threads{
	int noThreads;					//number of threads
	pthread_t* tids; 				// execution threads
	pthread_cond_t notEmpty;		//cond var for checking if queue is empty
	pthread_cond_t notFull;			//cond var for checking if queue is full
	pthread_mutex_t lockData;		//mutex for locking queue
	pthread_mutex_t lockAdditional;		//mutex for locking additional data like generalInfo alterations
	linksQueue* queue;					//queue that the threads alter
	generalInfo* info;					//helpful for argument
	char* saveDir;						//helpful for argument
	char* startingUrl;					//helpful for argument
	char* hostIP;						//helpful for argument
	createdLinks* created;				//helpful for argument
	int servingPort;					//helpful for argument
	struct sockaddr *serverptr;
};

//functions for pool data
linkNode* popFromQueue(threads* th);
void insertQueue(threads* th, char* link);

//functions for threads
threads* initializeThreads(int numThreads, generalInfo* info, char* saveDir, char* startingUrl, char* hostIP, linksQueue* queue, createdLinks* created, int servingPort);
void* connectHandler(void* args);
void destroyThreads(threads** th);

#endif