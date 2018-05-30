CC = gcc -g -lm
FILES = main.c variousMethods.c linksQueue.c
OBJECTS = main.o variousMethods.o linksQueue.o
OUT = mycrawler
HEADERS = variousMethods.h linksQueue.h

all: $(OBJECTS) $(HEADERS)
	$(CC) -o $(OUT) $(FILES) -lm
	make clean
	
main.o: main.c
	$(CC) -c main.c

variousMethods.o: variousMethods.c
	$(CC) -c variousMethods.c
	
linksQueue.o: linksQueue.c
	$(CC) -c linksQueue.c
	
clean:
	rm -f $(OBJECTS)