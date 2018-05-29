#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "httpRequests.h"

int checkRequestInfo(char* request, char* rootDir, request** info){
	int lines = 0;
	int length = 0;
	int first = 1;
	for(int i=0;i<strlen(request);i++){
		char ch = request[i];
		length++;
		if(ch == '\n'){
			char line[length];
			memcpy(line,request + lines,length);
			line[length-1] = '\0';
			printf("line: '%s'\n",line);
			lines = i+1;
			length = 0;
			//edit line
			if(first){
				first = 0;
			}else{
				
			}
		}
	}
}

void saveRequestInfo