CC = gcc -g -lm -pthread
FILES = main.c variousMethods.c linksQueue.c generalInfo.c threadPool.c
OBJECTS = main.o variousMethods.o linksQueue.o generalInfo.o threadPool.o
OUT = mycrawler
HEADERS = variousMethods.h linksQueue.h generalInfo.h threadPool.h

all: $(OBJECTS) $(HEADERS)
	$(CC) -o $(OUT) $(FILES) -lm
	make subsystem
	make clean
	
main.o: main.c
	$(CC) -c main.c

variousMethods.o: variousMethods.c
	$(CC) -c variousMethods.c
	
linksQueue.o: linksQueue.c
	$(CC) -c linksQueue.c
	
generalInfo.o: generalInfo.c
	$(CC) -c generalInfo.c
	
threadPool.o: threadPool.c
	$(CC) -c threadPool.c
	
subsystem:
	cd ../jobExecutor && $(MAKE)
	
clean:
	rm -f $(OBJECTS)