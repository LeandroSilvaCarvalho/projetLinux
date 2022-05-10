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
#include "transfer.h"

int initSocketServer(int port)
{
  int sockfd = ssocket();
  sbind(port, sockfd);
  slisten(sockfd, BACKLOG);
  return sockfd;
}

int main(int argc, char const *argv[])
{
  StructMessage msg;
  if (argc < 1)
  {
    perror("Missing arguments\n");
    exit(0);
  }

  int sem_id = sem_get(SEM_KEY, 1);
  int shm_id = sshmget(SHM_KEY, 1000 * sizeof(int), 0);
  int *z = sshmat(shm_id);

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

  while (1)
  {
    spoll(fds, nbSockfd, 0);

    if (fds[0].revents & POLLIN & !fds_invalid[0])
    {
      newSockfd = saccept(sockfd);
      fds[nbSockfd].fd = newSockfd;
      fds[nbSockfd].events = POLLIN;
      fds_invalid[nbSockfd] = false;
      nbSockfd++;

      sread(newSockfd, &msg, sizeof(msg));

      int sizeTransfers = msg.sizeTransfers;
      for (int i = 0; i < sizeTransfers; i++)
      {
        StructTransfer transfer = msg.transfers[i];
        int sender = transfer.sender;
        int receiver = transfer.receiver;
        int amount = transfer.amount;

        printf("The sender: %d\n", sender);
        printf("The receiver: %d\n", receiver);
        printf("The amount: %d\n", amount);

        if (z[sender] - amount < LIMIT_AMOUNT)
        {
          perror("The sender does not have a sufficient balance \n");
        }
        else
        {
          sem_down0(sem_id);
          z[sender] -= amount;
          z[receiver] += amount;
          sem_up0(sem_id);
          msg.newSolde = z[sender];
        }
      }
    }

    msg.code = INSCRIPTION_OK;

    swrite(newSockfd, &msg, sizeof(msg));
  }
  sclose(sockfd);
  exit(EXIT_SUCCESS);
}
