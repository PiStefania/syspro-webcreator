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

linksQueue* createLinksQueue();
void pushLinksQueue(linksQueue* queue,char* link);
linkNode* popLinksQueue(linksQueue* queue);
void destroyLinksQueue(linksQueue** queue);
void destroyLinkNode(linkNode** node);
int isEmptyLinksQueue(linksQueue* queue);
int checkIfExistsLinksQueue(linksQueue* queue, char* link);
void printLinksQueue(linksQueue* queue);
char* convertToLink(char* startingUrl);
char* createRequest(char* link, char* host);
void insertLinksQueueContent(linksQueue* queue, char* content, char* site);

#endif