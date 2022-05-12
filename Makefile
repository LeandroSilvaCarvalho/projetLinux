##### @authors: DIDI Abdenour, PHAM Van-Huy, SILVA DE CARVALHO Leandro #####

CC=gcc

CCFLAGS=-D_DEFAULT_SOURCE -D_XOPEN_SOURCE -D_BSD_SOURCE -std=c11 -pedantic -Wvla -Wall -Werror -g

ALL= client server pdr maint

all: $(ALL)

client : client.o utils_v1.o
	$(CC) $(CCFLAGS) -o client client.o utils_v1.o
client.o: client.c utils_v1.h
	$(CC) $(CCFLAGS) -c client.c

server : server.o utils_v1.o
	$(CC) $(CCFLAGS) -o server server.o utils_v1.o
server.o: server.c utils_v1.h
	$(CC) $(CCFLAGS) -c server.c

pdr : pdr.o utils_v1.o
	$(CC) $(CCFLAGS) -o pdr pdr.o utils_v1.o
pdr.o: pdr.c utils_v1.h
	$(CC) $(CCFLAGS) -c pdr.c

maint : maint.o utils_v1.o
	$(CC) $(CCFLAGS) -o maint maint.o utils_v1.o
maint.o: maint.c utils_v1.h
	$(CC) $(CCFLAGS) -c maint.c

utils_v1.o: utils_v1.c utils_v1.h
	$(CC) $(CCFLAGS) -c utils_v1.c

clean:
	rm -f *.o
	rm -f $(ALL)