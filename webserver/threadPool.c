#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "threadPool.h"
#include "variousMethods.h"
#define POOL_DATA_SIZE 20

//functions for pool data
poolData* initializePoolData(){
	poolData* data = malloc(sizeof(poolData));
	data->bufferFds = malloc(POOL_DATA_SIZE*sizeof(int));
	data->end = -1;
	data->position = 0;
}

void insertPoolData(threads* th, int fd){
	pthread_mutex_lock(&(th->lockData));
	while (th->data->position >= POOL_DATA_SIZE) {
		pthread_cond_wait(&(th->notFull), &(th->lockData));
	}
	//insert to buffer
	th->data->end = (th->data->end + 1) % POOL_DATA_SIZE;
	th->data->bufferFds[th->data->end] = fd;
	th->data->position++;
	pthread_mutex_unlock(&(th->lockData));
}


int getPoolData(threads* th){
	int fd = 0;
	pthread_mutex_lock(&(th->lockData));
	while(th->data->position <= 0) {
		pthread_cond_wait(&(th->notEmpty), &(th->lockData));
	}
	//get first elem 
	fd = th->data->bufferFds[0];
	//shift elems
	for (int i=1;i < th->data->end;i++)
  		th->data->bufferFds[i-1] = th->data->bufferFds[i];
	th->data->position--;
	pthread_mutex_unlock(&(th->lockData));
	return fd;
}

void destroyPoolData(poolData** data){
	free((*data)->bufferFds);
	(*data)->bufferFds = NULL;
	(*data)->end = -1;
	(*data)->position = 0;
	free(*data);
	*data = NULL;
}

//functions for threads
threads* initializeThreads(int numThreads, generalInfo* info, char* rootDir){
	threads* th = malloc(sizeof(threads));
	th->tids = malloc(numThreads*sizeof(pthread_t));
	th->noThreads = numThreads;
	th->info = info;
	th->rootDir = rootDir;
	
	if(pthread_mutex_init(&(th->lockData),NULL)!=0){
		//destroy threads
		destroyThreads(&th);
		return NULL;
	}
	
	if(pthread_cond_init(&(th->notEmpty),NULL)!=0){
		//destroy threads
		destroyThreads(&th);
		return NULL;
	}
	
	if(pthread_cond_init(&(th->notFull),NULL)!=0){
		//destroy threads
		destroyThreads(&th);
		return NULL;
	}
	
	//initialize poolData
	th->data = initializePoolData();
	
	for(int i=0;i<numThreads;i++){
		if(pthread_create(&th->tids[i],NULL, (void*) acceptHandler, th)!=0){
			//destroy threads
			destroyThreads(&th);
			return NULL;
		}
	}
	return th;
}

void* acceptHandler(void* args){
	threads* th = (threads*) args;
	while(1){
		//get fd
		int newsock = getPoolData(th);
		printf("newsock in thread: %d\n",newsock);
		readGetLinesFromCrawler(newsock,th->rootDir,th->info);
		close(newsock);
		pthread_cond_signal(&th->notFull);
	}
	pthread_exit(NULL);
}

void destroyThreads(threads** th){
	for(size_t i = 0; i<(*th)->noThreads; i++) {
        pthread_join((*th)->tids[i], NULL);
    }
			
	if((*th)->tids!=NULL){
		free((*th)->tids);
		(*th)->tids = NULL;
	}
	
	(*th)->noThreads = -1;
	
	pthread_mutex_lock(&((*th)->lockData));
    pthread_mutex_destroy(&((*th)->lockData));
    pthread_cond_destroy(&((*th)->notEmpty));
    pthread_cond_destroy(&((*th)->notFull));
	
	destroyPoolData(&((*th)->data));
	
	free((*th));
	*th = NULL;
}

