CC=gcc
CFLAGS=-I

all: sem_cli sem_svr

sem_cli: sem_cli.c sem_cli.h
	$(CC) -o sem_cli sem_cli.c

sem_svr: sem_svr.c sem_svr.h
	$(CC) -o sem_svr sem_svr.c

clean:
	rm sem_cli sem_svr