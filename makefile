CC=gcc
CFLAGS=-I.

stat: statFile.o q.o
	$(CC) -o statFile statFile.o q.o -I.
