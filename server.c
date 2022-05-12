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

/**
 * PRE:  port: a valid port number
 * POST: on success, binds a socket to 0.0.0.0:port and listen to it
 *       return socket file descriptor
 *       on failure, displays the error cause and quits the program
 */
int initSocketServer(int port)
{
  int sockfd = ssocket();
  sbind(port, sockfd);
  slisten(sockfd, BACKLOG);
  return sockfd;
}

//******************************************************************************
// Signal handler
//******************************************************************************
void sigint_handler()
{
  printf("\nShutting down the server...\n");
  end = 1;
}

//******************************************************************************
// MAIN FUNCTION
//******************************************************************************
int main(int argc, char const *argv[])
{
  // Signal arming SIGINT
  ssigaction(SIGINT, sigint_handler);
  StructMessage msg;
  if (argc < 1)
  {
    perror("Missing arguments\n");
    exit(EXIT_FAILURE);
  }

  // Getting semaphores
  int sem_id = sem_get(SEM_KEY, 1);
  // Getting the shared memory
  int shm_id = sshmget(SHM_KEY, 1000 * sizeof(int), 0);
  int *sharedMem = sshmat(shm_id);

  int port = atoi(argv[1]);
  int sockfd = initSocketServer(port);
  printf("The server started on port: %d\n", port);
  printf("\n");

  while (end == 0)
  {
    // Accepts a client connection
    int newSockfd = accept(sockfd, NULL, NULL);
    if (newSockfd > -1)
    {
      // Reads the message from the client
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

          // We decided to set a balance limit which is -500€
          // so the account's balance can't go below that limit
          if (sharedMem[sender] - amount < LIMIT_AMOUNT)
          {
            perror("The sender doesn't have a sufficient balance\n");
            strcpy(msg.message, "The sender doesn't have a sufficient balance");
            msg.code = TRANSFER_KO;
          }
          else
          {
            sem_down0(sem_id);
            // Start of critical section
            printf("Sender's account number: %d\n", sender);
            printf("Receiver's account number: %d\n", receiver);
            printf("Amount transferred to the receiver: %d€\n", amount);
            sharedMem[sender] -= amount;
            sharedMem[receiver] += amount;
            // End of critical section
            sem_up0(sem_id);
            msg.newBalance = sharedMem[sender];
            msg.code = TRANSFER_OK;
          }
          printf("New balance of the sender's account: %d€\n", msg.newBalance);
          printf("\n");
        }
        swrite(newSockfd, &msg, sizeof(msg));
      }

      // Close client's connection
      sclose(newSockfd);
    }
  }

  // Segment detached
  sshmdt(sharedMem);
  // Close listening socket
  sclose(sockfd);
  exit(EXIT_SUCCESS);
}
