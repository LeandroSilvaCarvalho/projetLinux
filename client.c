#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#include "utils_v1.h"
#include "transfer.h"
#include "messages.h"

#define CHOICE_SIZE 50  // Size of the buffer for the user's inputs

/**
 * PRE: ServerIP: a valid IP address
 *      Serverport: a valid port number
 * POST: on success, connects a client socket to ServerIP:port
 *       on failure, displays the error cause and quits the program
 */
int initSocketClient(char ServerIP[16], int Serverport)
{
    int sockfd = ssocket();
    sconnect(ServerIP, Serverport, sockfd);
    return sockfd;
}

//******************************************************************************
// Displays the available proposals to the user
//******************************************************************************
void proposals()
{
    printf("Proposals:\n");
    printf("To do a transfer: + [receiver] [amount]\n");
    printf("To do a recurring transfer: * [receiver] [amount]\n");
    printf("To quit the program: press q\n");
}

//******************************************************************************
// Timer child code sending a heartbeat every delay seconds
//******************************************************************************

/**
 * PRE: argDelay: the delay for each heartbeat
 *      argPipe: the pipe where the heartbeat will be send
 * POST: on success, sends the heartbeat in the pipe
 */
void child_timer(void *argDelay, void *argPipe)
{
    int *delay = argDelay;
    int *pipe = argPipe;

    // Closing the reading descriptor
    sclose(pipe[0]);
    while (1)
    {
        sleep(*delay);
        StructTransfer transfer;
        transfer.receiver = -1;
        // Writing in the pipe
        swrite(pipe[1], &transfer, sizeof(transfer));
    }

    exit(EXIT_SUCCESS);
}

//******************************************************************************
// Second child code which manages a series of recurring transfers 
// at each heartbeat sent by the timer child
//******************************************************************************

/**
 * PRE: argPipe: the pipe where the data is sent
 *      argAddress: the server's address 
 *      argPort: the server's port
 * POST: sends the array to the server if there's a receiver (-1)
 *       otherwise, put the recurrent transfer in an array
 */
void child_recurrent_transfer(void *argPipe, void *argAddress, void *argPort)
{
    int *pipe = argPipe;
    char *adr = argAddress;
    int *port = argPort;

    // Closing the writing descriptor
    sclose(pipe[1]);
    StructTransfer transfers[100];
    int sizeTransfers = 0;

    int nbRd;
    StructTransfer transfer;
    
    // Reading in the pipe
    while ((nbRd = sread(pipe[0], &transfer, sizeof(transfer))) != 0)
    {
        if (transfer.receiver == -1)
        {
            StructMessage message;
            message.sizeTransfers = sizeTransfers;
            int sockfd = initSocketClient(adr, *port);
            for (int i = 0; i < sizeTransfers; i++)
            {
                message.transfers[i] = transfers[i];
            }
            swrite(sockfd, &message, sizeof(message));
            sclose(sockfd);
        }
        else
        {
            transfers[sizeTransfers] = transfer;
            sizeTransfers++;
        }
    }

    // Closing the reading descriptor
    sclose(pipe[0]);
    exit(EXIT_SUCCESS);
}

//******************************************************************************
// MAIN FUNCTION
//******************************************************************************
int main(int argc, char const *argv[])
{
    if (argc != 5)
    {
        perror("Missing arguments\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        char adr[16];
        strcpy(adr, argv[1]);
        int port = atoi(argv[2]);
        int num = atoi(argv[3]);
        int delay = atoi(argv[4]);
        printf("Address: %s\n", adr);
        printf("Port: %d\n", port);
        printf("Sender's account number: %d\n", num);
        printf("Delay for each reccurrent transfer: %ds\n", delay);
        printf("\n");

        StructTransfer transfer;
        transfer.sender = num;

        int pipe[2];
        // Creation of the pipe
        spipe(pipe);

        // Creation of both children
        int timerId = fork_and_run2(child_timer, &delay, &pipe);
        int recurrentTrasferId = fork_and_run3(child_recurrent_transfer, &pipe, &adr, &port);

        // Closing the reading descriptor
        sclose(pipe[0]);
        proposals();

        // Array to split with strtok
        char choice[CHOICE_SIZE];
        sread(0, choice, CHOICE_SIZE);
        while (choice[0] != 'q')
        {
            if (strlen(choice) > 1)
            {
                // Splits the array with space as delimiter 
                char *symbol = strtok(choice, " ");
                // We pass NULL as the first argument to continue with the same string
                // and to get the next token (because of the static pointer)
                char *receiver = strtok(NULL, " ");
                char *amount = strtok(NULL, "\n");
                
                printf("\n");
                if (strcmp(symbol, "+") == 0)
                {
                    printf("+++++ New transfer +++++\n");

                    int sockfd = initSocketClient(adr, port);
                    transfer.receiver = atoi(receiver);
                    transfer.amount = atoi(amount);

                    StructMessage message;
                    message.transfers[0] = transfer;
                    message.sizeTransfers = 1;
                    swrite(sockfd, &message, sizeof(message));

                    // Waiting for a server response
                    sread(sockfd, &message, sizeof(message));
                    if (message.code == TRANSFER_OK)
                    {
                        printf("The transfer has been done successfully\n");
                        int newBalance = message.newBalance;
                        printf("The new balance of your account is: %d€\n", newBalance);
                    }
                    else
                    {
                        printf("An error occurred\n");
                        printf("%s\n", message.message);
                        printf("The transfer hasn't been done\n");
                    }
                    sclose(sockfd);
                }
                else if (strcmp(symbol, "*") == 0)
                {
                    printf("***** New recurrent transfer *****\n");
                    transfer.receiver = atoi(receiver);
                    transfer.amount = atoi(amount);
                    swrite(pipe[1], &transfer, sizeof(transfer));
                }
                printf("\n");
            }
            proposals();
            sread(0, choice, CHOICE_SIZE);
        }
        
        // Resource freed when q is pressed
        sclose(pipe[1]);
        skill(timerId, SIGKILL);
        skill(recurrentTrasferId, SIGKILL);
    }

    printf("See you next time!\n");
    exit(EXIT_SUCCESS);
}
