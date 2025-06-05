CC=gcc
CFLAGS=-Wall -std=c99
EXEC=projet

all: $(EXEC)

projet: projet.o
	$(CC) projet.o -lm -o projet && rm -rf *.o

projet.o: projet.c projet.h
	$(CC) $(CFLAGS) -c projet.c -o projet.o 

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)