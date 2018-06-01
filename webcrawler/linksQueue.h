#ifndef _LINKS_QUEUE_H_
#define _LINKS_QUEUE_H_

typedef struct linkNode linkNode;

struct linkNode{
	char* link;
	linkNode* next;
};

typedef struct linksQueue{
	int size;
	linkNode* first;
	linkNode* last;
}linksQueue;

typedef struct createdLinks{
	char** createdArray;
	int size;
	int position;
}createdLinks;

//linksQueue functions
linksQueue* createLinksQueue();
void pushLinksQueue(linksQueue* queue,char* link, createdLinks* created);
linkNode* popLinksQueue(linksQueue* queue);
void destroyLinksQueue(linksQueue** queue);
void destroyLinkNode(linkNode** node);
int isEmptyLinksQueue(linksQueue* queue);
int checkIfExistsLinksQueue(linksQueue* queue, char* link);
void printLinksQueue(linksQueue* queue);
//functions for links
char* convertToLink(char* startingUrl);
char* createRequest(char* link, char* host);
void insertLinksQueueContent(linksQueue* queue, char* content, char* site, createdLinks* created);
//functions for createdLinks
createdLinks* createCreatedLinks();
void doubleCreatedLinks(createdLinks* created);
void insertCreatedLinks(createdLinks* created, char* link);
int binarySearchCreatedLinks(createdLinks* created, char* link);
void printCreatedLinks(createdLinks* created);
void destroyCreatedLinks(createdLinks** created);
int binaryCheckCreated(createdLinks* created, char* link);

#endif