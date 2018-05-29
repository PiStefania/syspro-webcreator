#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "httpRequests.h"
#include "variousMethods.h"

static char* HEADERS[] = {"User-Agent","Host","Accept-Language","Accept-Encoding","Connection"};
static char FILE_OK[] = {"HTTP/1.1 200 OK\n"};
static char FILE_NOT_FOUND[] = {"HTTP/1.1 404 Not Found\n"};
static char FILE_NOT_AUTHORIZED[] = {"HTTP/1.1 403 Forbidden\n"};

int checkRequestInfo(char* request, char** file){
	int lines = 0;
	int length = 0;
	int first = 1;
	int ret = 0;
	int hostFlag = 0;
	for(int i=0;i<strlen(request);i++){
		char ch = request[i];
		length++;
		if(ch == '\n'){
			char line[length];
			memcpy(line,request + lines,length);
			line[length-1] = '\0';
			lines = i+1;
			length = 0;
			//edit line
			if(first){
				char* token = strtok(line," ");
				if(strcmp(token,"GET")==0){
					token = strtok(NULL," ");
					*file = malloc((strlen(token)+1)*sizeof(char));
					strcpy(*file,token);
					token = strtok(NULL," ");
					if(strcmp(token,"HTTP/1.1")!=0){
						free(*file);
						*file = NULL;
					}
				}else{
					*file = NULL;
				}
				first = 0;
			}else{
				char* token = strtok(line,":");
				if(strcmp("Host",token)==0){
					hostFlag=1;
				}
				ret = checkHeader(token);
				if(!ret){
					return 0;
				}
			}
		}
	}
	
	if(!hostFlag){
		return 0;
	}
}

int checkHeader(char* header){
	int length = sizeof(HEADERS) / sizeof(HEADERS[0]);
	for(int i=0;i<length;i++){
		if(strcmp(HEADERS[i],header)==0){
			return 1;
		}
	}
	return 0;
}

char* constructResponse(char* fullPath){
	int fullLength = 0;
	char* response = NULL;
	if(access(fullPath, F_OK) != -1){
		if(access(fullPath, R_OK) != -1){
			fullLength += strlen(FILE_OK);
			//get characters of file
			FILE *fp = fopen(fullPath,"r");
			fseek(fp, 0L, SEEK_END);
			int size = ftell(fp);
			fseek(fp, 0L, SEEK_SET);
			int lengthSize = getNumberLength(size);
			//add time and headers
			char* timeBuf = insertCurrentTime(&fullLength);
			char* additionalBuf = insertAdditionalHeaders(&fullLength,lengthSize,size);
			char* content = malloc((size)*sizeof(char));
			fread(content, 1, size, fp);
			//copy to response
			fullLength += size;
			response = malloc(fullLength*sizeof(char));
			strcpy(response,FILE_OK);
			strcat(response,timeBuf);
			strcat(response,additionalBuf);
			strcat(response,content);
			response[fullLength] = '\0';
			fclose(fp);
		}else{
			fullLength += strlen(FILE_NOT_AUTHORIZED);
			//add time and headers
			char* timeBuf = insertCurrentTime(&fullLength);
			char* content = insertHtmlNotAuth(&fullLength);
			int lengthContent = strlen(content);
			int lengthNumberContent = getNumberLength(lengthContent);
			char* additionalBuf = insertAdditionalHeaders(&fullLength,lengthNumberContent,lengthContent);
			response = malloc(fullLength*sizeof(char));
			strcpy(response,FILE_NOT_AUTHORIZED);
			strcat(response,timeBuf);
			strcat(response,additionalBuf);
			strcat(response,content);
			response[fullLength] = '\0';
		}
	}else{
		fullLength += strlen(FILE_NOT_FOUND);
		//add time and headers
		char* timeBuf = insertCurrentTime(&fullLength);
		char* content = insertHtmlNotFound(&fullLength);
		int lengthContent = strlen(content);
		int lengthNumberContent = getNumberLength(lengthContent);
		char* additionalBuf = insertAdditionalHeaders(&fullLength,lengthNumberContent,lengthContent);
		response = malloc(fullLength*sizeof(char));
		strcpy(response,FILE_NOT_FOUND);
		strcat(response,timeBuf);
		strcat(response,additionalBuf);
		strcat(response,content);
		response[fullLength] = '\0';
	}
	return response;
}

char* insertCurrentTime(int* length){
	char* timeBuf = malloc(50*sizeof(char));
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(timeBuf,50,"Date: %a, %d %B %Y %H:%M:%S GMT\n",timeinfo);
	*length += strlen(timeBuf);
	return timeBuf;
}

char* insertAdditionalHeaders(int* length,int contentLength, int size){
	int sizeAdditional = strlen("Server: myhttpd/1.0.0 (Ubuntu64)\nContent-Length: \nContent-Type: text/html\nConnection: Closed\n\n") + contentLength;
	char* additionalBuf = malloc(sizeAdditional*sizeof(char));
	sprintf(additionalBuf,"Server: myhttpd/1.0.0 (Ubuntu64)\nContent-Length: %d\nContent-Type: text/html\nConnection: Closed\n\n",size);
	*length += sizeAdditional;
	return additionalBuf;
}

char* insertHtmlNotFound(int* length){
	int lengthContent = strlen("<html>Sorry dude, couldn’t find this file.</html>\n");
	*length += lengthContent;
	char* content = malloc(lengthContent*sizeof(char));
	strcpy(content,"<html>Sorry dude, couldn’t find this file.</html>\n");
	return content;
}

char* insertHtmlNotAuth(int* length){
	int lengthContent = strlen("<html>Trying to access this file but don’t think I can make it.</html>\n");
	*length += lengthContent;
	char* content = malloc(lengthContent*sizeof(char));
	strcpy(content,"<html>Trying to access this file but don’t think I can make it.</html>\n");
	return content;
}