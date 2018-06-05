#ifndef _LINKS_QUEUE_H_
#define _LINKS_QUEUE_H_

typedef struct linkNode linkNode;
typedef struct linksQueue linksQueue;
typedef struct createdLinks createdLinks;

struct linkNode{
	char* link;
	linkNode* next;
};

struct linksQueue{
	int size;
	linkNode* first;
	linkNode* last;
};

struct createdLinks{
	char** createdArray;
	int size;
	int position;
};

#include "threadPool.h"

//linksQueue functions
linksQueue* createLinksQueue();
void pushLinksQueue(char* link, threads* th);
linkNode* popLinksQueue(threads* th);
void destroyLinksQueue(linksQueue** queue);
void destroyLinkNode(linkNode** node);
int isEmptyLinksQueue(linksQueue* queue);
int checkIfExistsLinksQueue(linksQueue* queue, char* link);
void printLinksQueue(linksQueue* queue);
//functions for links
char* convertToLink(char* startingUrl);
char* createRequest(char* link, char* host);
void insertLinksQueueContent(char* content, char* site, threads* th);
//functions for createdLinks
createdLinks* createCreatedLinks();
void doubleCreatedLinks(createdLinks* created);
void insertCreatedLinks(createdLinks* created, char* link);
int binarySearchCreatedLinks(createdLinks* created, char* link);
void printCreatedLinks(createdLinks* created);
void destroyCreatedLinks(createdLinks** created);
int binaryCheckCreated(createdLinks* created, char* link);

#endif