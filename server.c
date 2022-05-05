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
    structMessage msg;
    if(argc<1){
        perror("Missing arguments\n");
        exit(0);
    }
    
    struct pollfd fds[1024];
    bool fds_invalid[1024];
    int nbSockfd = 0;

    int port = atoi(argv[1]);
    int sockfd = initSocketServer(port);
    printf("Le serveur tourne sur le port : %i \n", port);
    
    fds[nbSockfd].fd = sockfd;
    fds[nbSockfd].events = POLLIN;
    fds_invalid[nbSockfd] = false;
    nbSockfd++;
    int newSockfd;


    while(1){
      spoll(fds, nbSockfd, 5000);

      if(fds[0].revents & POLLIN & !fds_invalid[0]){
        newSockfd = saccept(sockfd);
        fds[nbSockfd].fd = newSockfd;
		    fds[nbSockfd].events = POLLIN;
		    fds_invalid[nbSockfd] = false;
        nbSockfd++;
        sread(newSockfd, &msg, sizeof(msg));
        printf("bonjour %s\n", msg.messageText);
        printf("aurevoir %s\n", msg.messageText);
        msg.code = INSCRIPTION_OK;
      }
      nwrite(newSockfd, &msg, sizeof(msg));
    }

    sclose(sockfd);
    
    return 0;
}
