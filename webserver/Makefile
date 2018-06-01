CC = gcc -g -lm
FILES = main.c variousMethods.c httpRequests.c generalInfo.c
OBJECTS = main.o variousMethods.o httpRequests.o generalInfo.o
OUT = myhttpd
HEADERS = variousMethods.h httpRequests.h generalInfo.h

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
	
clean:
	rm -f $(OBJECTS)