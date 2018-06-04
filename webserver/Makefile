CC = gcc -g -lm -pthread
FILES = main.c variousMethods.c httpRequests.c generalInfo.c threadPool.c
OBJECTS = main.o variousMethods.o httpRequests.o generalInfo.o threadPool.o
OUT = myhttpd
HEADERS = variousMethods.h httpRequests.h generalInfo.h threadPool.h

all: $(OBJECTS) $(HEADERS)
	$(CC) -o $(OUT) $(FILES) -lm
	make clean
	
main.o: main.c
	$(CC) -c main.c

variousMethods.o: variousMethods.c
	$(CC) -c variousMethods.c
	
httpRequests.o: httpRequests.c
	$(CC) -c httpRequests.c
	
generalInfo.o: generalInfo.c
	$(CC) -c generalInfo.c
	
threadPool.o: threadPool.c
	$(CC) -c threadPool.c
	
clean:
	rm -f $(OBJECTS)