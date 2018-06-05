#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 		// sockets
#include <sys/socket.h> 	// sockets
#include <netinet/in.h> 	// internet sockets
#include <unistd.h> 		// read, write, close
#include <netdb.h> 			// gethostbyaddr
#include <string.h> 
#include <fcntl.h> 
#include <errno.h> 
#include "threadPool.h"
#include "variousMethods.h"
#define POOL_DATA_SIZE 20

void insertQueue(threads* th, char* link){
	pthread_mutex_lock(&(th->lockData));
	while (th->queue->size >= POOL_DATA_SIZE) {
		printf("WAIT FOR POP TO HAPPEN\n");
		pthread_cond_wait(&(th->notFull), &(th->lockData));
	}
	//insert to queue
	printf("PUSH LINK\n");
	pushLinksQueue(link, th);
	pthread_mutex_unlock(&(th->lockData));
}

linkNode* popFromQueue(threads* th){
	int fd = 0;
	pthread_mutex_lock(&(th->lockData));
	while(th->queue->size <= 0) {
		printf("WAIT FOR PUSH TO HAPPEN\n");
		pthread_cond_wait(&(th->notEmpty), &(th->lockData));
	}
	//get first elem 
	printf("POP NODE\n");
	linkNode* node = popLinksQueue(th);
	pthread_mutex_unlock(&(th->lockData));
	return node;
}

//functions for threads
threads* initializeThreads(int numThreads, generalInfo* info, char* saveDir, char* startingUrl, char* hostIP, linksQueue* queue, createdLinks* created, int servingPort){
	threads* th = malloc(sizeof(threads));
	th->tids = malloc(numThreads*sizeof(pthread_t));
	th->noThreads = numThreads;
	th->info = info;
	th->saveDir = saveDir;
	th->startingUrl = startingUrl;
	th->hostIP = hostIP;
	th->queue = queue;
	th->created = created;
	th->servingPort = servingPort;
	
	if(pthread_mutex_init(&(th->lockData),NULL)!=0){
		//destroy threads
		destroyThreads(&th);
		return NULL;
	}
	
	if(pthread_mutex_init(&(th->lockAdditional),NULL)!=0){
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
	
	for(int i=0;i<numThreads;i++){
		if(pthread_create(&th->tids[i],NULL, (void*) connectHandler, th)!=0){
			//destroy threads
			destroyThreads(&th);
			return NULL;
		}
	}
	return th;
}

void* connectHandler(void* args){
	threads* th = (threads*) args;
	int sock, i;
	struct sockaddr_in server;
	struct sockaddr *serverptr = (struct sockaddr*)&server;
	struct hostent *rem;
	
	if ((rem = gethostbyname(th->hostIP)) == NULL){			// Find server address
		herror("gethostbyname"); 
		exit(1);
	}
	
	while(1){
		//create socket and connect with server
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){		// Create socket
			perror("socket");
			exit(1);
		}
		server.sin_family = AF_INET; 							// Internet domain 
		memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
		server.sin_port = htons(th->servingPort); 							// Server port 
		if (connect(sock, serverptr, sizeof(server)) < 0){		// Initiate connection 
			perror("connect");
			exit(1);
		}
		printf("SOCK: %d CLOSE\n",sock);
		//read lines and send to server
		readGetLinesFromServer(sock,th);
		close(sock);
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
	
    pthread_mutex_destroy(&((*th)->lockData));
    pthread_mutex_destroy(&((*th)->lockAdditional));
    pthread_cond_destroy(&((*th)->notEmpty));
    pthread_cond_destroy(&((*th)->notFull));
		
	free((*th));
	*th = NULL;
}

