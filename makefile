CC=gcc
CFLAGS=-I. -lpthread

stat: statFile.o q.o
	$(CC) -o statFile statFile.o q.o $(CFLAGS)
