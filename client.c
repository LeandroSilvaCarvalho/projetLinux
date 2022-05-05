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

    StructMessage msg;
    int ret = sread(0, msg.messageText, MAX_TEXT);
    msg.messageText[ret-1] = '\0';
    msg.code = INSCRIPTION_REQUEST;

    int sockfd = initSocketClient(argv[1], port);
    printf("indiquez votre compte : ");
    swrite(sockfd, &msg.senderAccount, sizeof(msg.senderAccount));
    printf("\nindiquez le compte beneficiaire : ");
    swrite(sockfd, &msg.beneficiaryAccount, sizeof(msg.beneficiaryAccount));
    printf("\n indiquez le montant : ");
    swrite(sockfd, &msg.amount, sizeof(msg.amount));
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
