CC = gcc -g -lm
FILES = main.c variousMethods.c linksQueue.c generalInfo.c
OBJECTS = main.o variousMethods.o linksQueue.o generalInfo.o
OUT = mycrawler
HEADERS = variousMethods.h linksQueue.h generalInfo.h

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
	
subsystem:
	cd ../jobExecutor && $(MAKE)
	
clean:
	rm -f $(OBJECTS)