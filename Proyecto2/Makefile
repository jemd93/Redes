CC=gcc
CFLAGS=-I

all: sem_cli sem_svr sem_svr_h

sem_cli: sem_cli.c sem_cli.h
	$(CC) -o sem_cli sem_cli.c

sem_svr: sem_svr.c sem_svr.h
	$(CC) -o sem_svr sem_svr.c

sem_svr_h: sem_svr_h.c sem_svr_h.h
	$(CC) -o sem_svr_h sem_svr_h.c

clean:
	rm sem_cli sem_svr sem_svr_h