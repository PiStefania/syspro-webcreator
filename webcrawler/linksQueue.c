#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linksQueue.h"
#define DEF_CREATED_SIZE 5

linksQueue* createLinksQueue(){
	linksQueue* queue = malloc(sizeof(linksQueue));
	queue->size = 0;
	queue->first = NULL;
	queue->last = NULL;
	return queue;
}

void pushLinksQueue(char* link, threads* th){
	if(binaryCheckCreated(th->created, link) == -1){
		if(th->queue->size==0 || th->queue->first == NULL){
			th->queue->first = malloc(sizeof(linkNode));
			th->queue->first->link = malloc((strlen(link)+1)*sizeof(char));
			strcpy(th->queue->first->link,link);
			th->queue->first->next = NULL;
			th->queue->last = th->queue->first;
			th->queue->size++;
			insertCreatedLinks(th->created,link);
		}else{
			th->queue->last->next = malloc(sizeof(linkNode));
			th->queue->last->next->link = malloc((strlen(link)+1)*sizeof(char));
			th->queue->last->next->next = NULL;
			strcpy(th->queue->last->next->link,link);
			th->queue->last = th->queue->last->next;
			th->queue->size++;
			insertCreatedLinks(th->created,link);
		}
		pthread_cond_signal(&(th->notEmpty));
	}
}

linkNode* popLinksQueue(threads* th){
	if(th->queue->size > 0){
		linkNode* firstNode = th->queue->first;
		th->queue->first = th->queue->first->next;
		th->queue->size--;
		pthread_cond_signal(&(th->notFull));
		return firstNode;
	}else{
		return NULL;
	}
}

void destroyLinksQueue(linksQueue** queue){
	linkNode* temp = (*queue)->first;
	for(int i=0;i<(*queue)->size;i++){
		free(temp->link);
		temp->link = NULL;
		free(temp);
		temp = temp->next;
	}
	(*queue)->last = NULL;
	(*queue)->size = 0;
	free(*queue);
	*queue = NULL;
}

void destroyLinkNode(linkNode** node){
	free((*node)->link);
	(*node)->link = NULL;
	(*node)->next = NULL;
	free(*node);
	*node = NULL;
}


int isEmptyLinksQueue(linksQueue* queue){
	if(queue->size == 0){
		return 1;
	}
	return 0;
}

void printLinksQueue(linksQueue* queue){
	linkNode* temp = queue->first;
	int i=0;
	while(temp!=NULL){
		printf("%d Node: '%s'\n",i,temp->link);
		temp = temp->next;
		i++;
	}
}

char* convertToLink(char* startingUrl){
	char* token = strtok(startingUrl,"/");
	token = strtok(NULL,"/");
	token = strtok(NULL,"/");
	char* site = malloc((strlen(token)+1)*sizeof(char));
	strcpy(site,token);
	token = strtok(NULL,"/");
	char* page = malloc((strlen(token)+1)*sizeof(char));
	strcpy(page,token);
	char* link = malloc((strlen(site) + strlen(page) + 3)*sizeof(char));
	sprintf(link,"/%s/%s",site,page);
	free(site);
	site = NULL;
	free(page);
	page = NULL;
	return link;
}

char* createRequest(char* link, char* host){
	int lengthFirstLine = strlen("GET  HTTP/1.1\n") + strlen(link) + 1;
	int lengthHostLine = strlen("Host: \n") + strlen(host) + 1;
	int lengthAdditionalLines=strlen("User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\nAccept-Language: en-us\nAccept-Encoding: gzip, deflate\nConnection: Keep-Alive\n\n")+1;
	int fullLength = lengthFirstLine + lengthHostLine + lengthAdditionalLines;
	char* request = malloc(fullLength*sizeof(char));
	sprintf(request,"GET %s HTTP/1.1\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\nHost: %s\nAccept-Language: en-us\nAccept-Encoding: gzip, deflate\nConnection: Keep-Alive\n",link,host);
	return request;
}

void insertLinksQueueContent(char* content, char* site, threads* th){
	char* resultStart = NULL;
	do{
		char* linkStart = "<a href=";
		resultStart = strstr(content, linkStart);
		if(resultStart == NULL)
			break;
		int positionStart = resultStart - content;
		char* linkEnd = "</a>";
		char* resultEnd = strstr(content, linkEnd);
		int positionEnd = resultEnd + strlen(linkEnd) - content;
		int subStringLength = positionEnd - positionStart;
		char link[subStringLength+1];
		for(int i=0;i<subStringLength;i++){
			link[i] = content[positionStart+i];
		}
		link[subStringLength] = '\0';
		//alter link
		char* page = strtok(link,"=");
		page = strtok(NULL,">");
		char* newPage = NULL;
		if(strstr(page,"../") != NULL){
			newPage = malloc((strlen(page)-1)*sizeof(char));
			memcpy(newPage,page+2,(strlen(page)-2));
			newPage[(strlen(page)-2)] = '\0';
		}else{
			newPage = malloc((strlen(page)+strlen(site)+3)*sizeof(char));
			sprintf(newPage,"/%s/%s",site,page);
		}
		//insert new page to queue
		insertQueue(th, newPage);
		free(newPage);
		newPage = NULL;
		content = resultEnd + strlen(linkEnd);
	} while(resultStart != NULL);
}

createdLinks* createCreatedLinks(){
	createdLinks* created = malloc(sizeof(createdLinks));
	created->size = DEF_CREATED_SIZE;
	created->position = 0;
	created->createdArray = malloc(created->size*sizeof(char*));
	for(int i=0;i<created->size;i++){
		created->createdArray[i] = NULL;
	}
	return created;
}

void doubleCreatedLinks(createdLinks* created){
	int oldSize = created->size;
	int newSize = oldSize + 5;
	created->createdArray = realloc(created->createdArray,newSize*sizeof(char*));
	for(int i=oldSize;i<newSize;i++){
		created->createdArray[i] = NULL;
	}
	created->size = newSize;
}

void insertCreatedLinks(createdLinks* created, char* link){
	int getPosition = binarySearchCreatedLinks(created,link);
	//full array
	if(created->position == created->size-1){
		doubleCreatedLinks(created);
	}
	
	if(created->position == 0){
		//empty
		created->createdArray[0] = malloc((strlen(link)+1)*sizeof(char));
		strcpy(created->createdArray[0],link);
		created->position++;
	}else{
		//same id
		if(created->createdArray[getPosition] != NULL){
			if(strcmp(link,created->createdArray[getPosition]) != 0){
				//need to shift other elements to insert a new node
				int fullMoveSize = (created->position - getPosition)*sizeof(created->createdArray[created->position]);
				memmove(&(created->createdArray[getPosition+1]), &(created->createdArray[getPosition]), fullMoveSize);
				created->createdArray[getPosition] = malloc((strlen(link)+1)*sizeof(char));
				strcpy(created->createdArray[getPosition],link);
				created->position++;
			}
		}else{
			created->createdArray[getPosition] = malloc((strlen(link)+1)*sizeof(char));
			strcpy(created->createdArray[getPosition],link);
			created->position++;
		}
	}
}

int binarySearchCreatedLinks(createdLinks* created, char* link){
	int first = 0;
	int last = created->position;
	if(last == 0)
		return 0;
	int mid = 0;
	char** array = created->createdArray;
	while (first < last){
        int mid = first + (last-first)/2;
        if(strcmp(array[mid],link) == 0)
            return mid;
        if (strcmp(array[mid],link) < 0)
            first = mid + 1;
		else
            last = mid - 1;
    }
	if(last<=first){
		if(array[first]!=NULL){
			if(0 < strcmp(link,array[first]))
				return first+1;
			else
				return first;
		}else{
			return first;
		}
	}
    return mid;
}

int binaryCheckCreated(createdLinks* created, char* link){
	int first = 0;
	int last = created->position-1;
	if(last == 0)
		return -1;
	int mid = 0;
	char** array = created->createdArray;
	while (first <= last){
        int mid = first + (last-first)/2;
        if(strcmp(array[mid],link) == 0){
            return mid;
		}
        if (strcmp(array[mid],link) < 0)
            first = mid + 1;
		else
            last = mid - 1;
    }
    return -1;
}
	
void printCreatedLinks(createdLinks* created){
	for(int i=0;i<created->position;i++){
		printf("Created: '%s'\n",created->createdArray[i]);
	}
}
	
void destroyCreatedLinks(createdLinks** created){
	for(int i=0;i<(*created)->position;i++){
		if((*created)->createdArray[i] != NULL){
			free((*created)->createdArray[i]);
			(*created)->createdArray[i] = NULL;
		}
	}
	free((*created)->createdArray);
	(*created)->createdArray = NULL;
	free(*created);
	*created = NULL;
}
