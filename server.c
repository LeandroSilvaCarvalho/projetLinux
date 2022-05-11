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

volatile sig_atomic_t end = 0;

int initSocketServer(int port)
{
  int sockfd = ssocket();
  sbind(port, sockfd);
  slisten(sockfd, BACKLOG);
  return sockfd;
}

void sigint_handler()
{
  printf("\nShutting down the server\n");
  end = 1;
}

int main(int argc, char const *argv[])
{
  ssigaction(SIGINT, sigint_handler);
  StructMessage msg;
  if (argc < 1)
  {
    perror("Missing arguments\n");
    exit(0);
  }

  int sem_id = sem_get(SEM_KEY, 1);
  int shm_id = sshmget(SHM_KEY, 1000 * sizeof(int), 0);
  int *z = sshmat(shm_id);

  int port = atoi(argv[1]);
  int sockfd = initSocketServer(port);
  printf("The server start on port: %d \n", port);

  while (end == 0)
  {
    int newSockfd = accept(sockfd, NULL, NULL);
    if (newSockfd > -1)
    {
      int nbCharRd = read(newSockfd, &msg, sizeof(msg));
      if (nbCharRd > -1)
      {
        int sizeTransfers = msg.sizeTransfers;
        for (int i = 0; i < sizeTransfers; i++)
        {
          StructTransfer transfer = msg.transfers[i];
          int sender = transfer.sender;
          int receiver = transfer.receiver;
          int amount = transfer.amount;
          printf("Sender: %d\n", sender);
          printf("receiver: %d\n", receiver);
          printf("amount: %d\n", amount);

          if (z[sender] - amount < LIMIT_AMOUNT)
          {
            perror("The sender does not have a sufficient balance \n");
            strcpy(msg.message, "The sender does not have a sufficient balance");
            msg.code = TRANSFER_KO;
          }
          else
          {
            sem_down0(sem_id);
            z[sender] -= amount;
            z[receiver] += amount;
            sem_up0(sem_id);
            msg.newSolde = z[sender];
            msg.code = TRANSFER_OK;
          }
          printf("New solde: %d\n", msg.newSolde);
        }
        swrite(newSockfd, &msg, sizeof(msg));
      }
      sclose(newSockfd);
    }
  }
  sclose(sockfd);
  exit(EXIT_SUCCESS);
}
