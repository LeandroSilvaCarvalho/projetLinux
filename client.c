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

int initSocketClient(char ServerIP[16], int Serverport)
{
    int sockfd = ssocket();
    sconnect(ServerIP, Serverport, sockfd);
    return sockfd;
}

int main(int argc, char *argv[])
{
    /*if(argc<4){
        perror("Missing argument");
        exit(0);
    }*/

    int port = atoi(argv[2]);
    char buffer [256];

    StructMessage msg;

    int sockfd = initSocketClient(argv[1], port);
    printf("indiquez votre compte : \n");
    int nb = sread(0,buffer, sizeof(buffer));
    buffer[nb-1]='\0';
    msg.senderAccount = atoi(buffer);


    printf("indiquez le compte beneficiaire : \n");
     nb = sread(0,buffer, sizeof(buffer));
    buffer[nb-1]='\0';
    msg.beneficiaryAccount = atoi(buffer);


    printf("indiquez le montant : \n");
     nb = sread(0,buffer, sizeof(buffer));
    buffer[nb-1]='\0';
    msg.amount = atoi(buffer);


    
    swrite(sockfd, &msg, sizeof(msg));

    sread(sockfd, &msg, sizeof(msg));
    if (msg.code == INSCRIPTION_OK)
    {
    printf("Réponse du serveur : Inscription acceptée\n");
  }
  else if (msg.code == INSCRIPTION_KO)
  {
    printf("Réponse du serveur : Inscription refusée\n");
  }

  sclose(sockfd);
    return 0;
}
