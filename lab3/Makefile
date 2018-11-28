# Author: Sean O'Donnell

all: lab3

lab3: shell.o shellA.o shellB.o
	gcc shell.o shellA.o shellB.o -o lab3

shell.o: shell.c shellA.h shellB.h
	gcc -ansi -pedantic -g -c shell.c -lpthread -lrt

shellA.o: shellA.c shellA.h
	gcc -ansi -pedantic -g -c shellA.c

shellB.o: shellB.c shellB.h
	gcc -ansi -pedantic -g -c shellB.c

clean:
	rm -rf *.o lab3
