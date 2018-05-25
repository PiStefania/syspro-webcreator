CC = gcc -g -lm
FILES = main.c variousMethods.c
OBJECTS = main.o variousMethods.o
OUT = mycrawler
HEADERS = variousMethods.h

all: $(OBJECTS) $(HEADERS)
	$(CC) -o $(OUT) $(FILES) -lm
	make clean
	
main.o: main.c
	$(CC) -c main.c

variousMethods.o: variousMethods.c
	$(CC) -c variousMethods.c
	
clean:
	rm -f $(OBJECTS)