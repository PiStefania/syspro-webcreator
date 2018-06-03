#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "generalInfo.h"

generalInfo* createGeneralInfo(){
	generalInfo* info = malloc(sizeof(generalInfo));
	info->millis = 0;
	info->startTime = 0;
	info->pagesDownloaded = 0;
	info->bytesDownloaded = 0;
	return info;
}

void printStats(generalInfo* info, int socket){
	//get duration
	struct timeb after;
	ftime(&after);
	long secs = (double)(after.time - info->startTime);
	int hours = secs / (60 * 60);
	int remainder = secs % 3600;
	long minutes = remainder / 60;
	long seconds = remainder % 60;
	
	char* buffer = malloc(100*sizeof(char));
	int millis = abs(after.millitm - info->millis);
	
	if(minutes > 10 && seconds > 10){
		sprintf(buffer,"Crawler up for 0%d:%ld:%ld.%d, downloaded %d pages, %ld bytes\n",hours,minutes,seconds,millis,info->pagesDownloaded,info->bytesDownloaded);
		if(write(socket,buffer,strlen(buffer)) < 0){
			perror("write command");
			free(buffer);
			buffer = NULL;
		}
	}else if(minutes > 10 && seconds < 10){
		sprintf(buffer,"Crawler up for 0%d:%ld:0%ld.%d, downloaded %d pages, %ld bytes\n",hours,minutes,seconds,millis,info->pagesDownloaded,info->bytesDownloaded);
		if(write(socket,buffer,strlen(buffer)) < 0){
			perror("write command");
			free(buffer);
			buffer = NULL;
		}
	}else if(minutes < 10 && seconds > 10){
		sprintf(buffer,"Crawler up for 0%d:0%ld:%ld.%d, downloaded %d pages, %ld bytes\n",hours,minutes,seconds,millis,info->pagesDownloaded,info->bytesDownloaded);
		if(write(socket,buffer,strlen(buffer))  < 0){
			perror("write command");
			free(buffer);
			buffer = NULL;
		}
	}else if(minutes < 10 && seconds < 10){
		sprintf(buffer,"Crawler up for 0%d:0%ld:0%ld.%d, downloaded %d pages, %ld bytes\n",hours,minutes,seconds,millis,info->pagesDownloaded,info->bytesDownloaded);
		if(write(socket,buffer,strlen(buffer)) < 0){
			perror("write command");
			free(buffer);
			buffer = NULL;
		}
	}
	free(buffer);
	buffer = NULL;
}

void executeJobExecutor(char* searchWords, char* saveDir, int socket){
	if(searchWords == NULL){
		printf("NO SEARCH WORDS FOUND\n");
		return;
	}
	
	char* writeBuf = malloc((strlen("/search ") + strlen("\n/exit\n") + strlen(searchWords) + strlen(" -d 100") + 1)*sizeof(char));
	sprintf(writeBuf,"/search %s -d 100\n/exit\n",searchWords);
	printf("writebuf: '%s'\n",writeBuf);
	//overwrite .
	char* saveD = malloc((strlen(saveDir)+1)*sizeof(char));
	strcpy(saveD,saveDir);
	memmove(saveD,saveD+1,strlen(saveD)-1);
	saveD[strlen(saveD)-1] = '\0';
	//create docFile from scratch
	system("rm -rf docfile");
	FILE *fp = fopen("docfile","w");
	if(fp != NULL){
		DIR* dir;
		struct dirent *ent;
		if((dir=opendir(saveDir)) != NULL){
			while (( ent = readdir(dir)) != NULL){
				if(ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0  && strcmp(ent->d_name, "..") != 0){
					//write to docfile
					char* subdirPath = malloc((strlen(saveD)+strlen("../webcrawler")+strlen(ent->d_name)+4)*sizeof(char));
					sprintf(subdirPath,"../webcrawler%s/%s/\n",saveD,ent->d_name);
					fwrite(subdirPath,1,strlen(subdirPath),fp);
					//free subdirPath
					free(subdirPath);
					subdirPath = NULL;
				}
			}
			closedir(dir);
		}
		fclose(fp);
		
		//call jobExecutor
		int pipesRead[2];
		int pipesWrite[2];
		if (pipe(pipesRead) == -1) {
			perror("pipe");
			exit(1);
		}
		if (pipe(pipesWrite) == -1) {
			perror("pipe");
			exit(1);
		}
		pid_t childpid = fork();
		int status;
		if(childpid < 0){
			perror("fork");
		}
		else if(childpid == 0){
			//child -> replace stdin with pipesRead[0]
			close(pipesRead[1]);
			dup2(pipesRead[0],STDIN_FILENO);    
        	close(pipesRead[0]);
			
			//child -> replace stdout with pipesWrite[1]
			close(pipesWrite[0]);
			dup2(pipesWrite[1],STDOUT_FILENO);    
        	close(pipesWrite[1]);
			
			//execute program
			char *args[]={"../jobExecutor/jobExecutor","-d","../webcrawler/docfile","-w","10",NULL};
			execv(args[0],args);
			perror("execv");
		}
		else{
			//parent -> write to stdin of child
            write(pipesRead[1], writeBuf, strlen(writeBuf));
        	close(pipesRead[1]); 
			close(pipesRead[0]);
			
			//parent -> get stdout of child
			while(1){
				int lengthMes = 0;
				int n;
				if(read(pipesWrite[0],&lengthMes,sizeof(int)) > 0){
					printf("lengthMes: %d\n",lengthMes);
					char readBuf[lengthMes]; 
					if(read(pipesWrite[0],readBuf,lengthMes) > 0){
						readBuf[lengthMes] = '\0';
						printf("rr: '%s'\n",readBuf);
						//write to socket
						write(socket,readBuf,lengthMes);
					}
				}else{
					printf("End\n");
					break;
				}
			}
			close(pipesWrite[0]);
			close(pipesWrite[1]);
			//wait for jobexecutor to finish
			waitpid(childpid,&status,0);
		}
	}else{
		printf("Error with creating/opening file\n");
	}
	
	//free vars
	free(saveD);
	saveD = NULL;
	free(writeBuf);
	writeBuf = NULL;
}