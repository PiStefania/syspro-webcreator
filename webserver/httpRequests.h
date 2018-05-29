#ifndef _HTTP_REQUESTS_H_
#define _HTTP_REQUESTS_H_

int checkRequestInfo(char* request, char* rootDir, request** info);

typedef struct request{
	char* page;
	char* host;
}request;

#endif