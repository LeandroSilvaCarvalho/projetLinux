#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "messages.h"
#include "utils_v1.h"
#include "config.h"

int initSocketServer(int port)
{
  int sockfd = ssocket();
  sbind(port, sockfd);
  slisten(sockfd, BACKLOG);
  return sockfd;
}

int main(int argc, char const *argv[])
{
    if(argc<1){
        perror("Missing arguments\n");
        exit(0);
    }
    int port = atoi(argv[1]);
    int sockfd = initSocketServer(port);
    printf("Le serveur tourne sur le port : %i \n", port);

    return 0;
}
