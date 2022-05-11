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

#define CHOICE_SIZE 50

int initSocketClient(char ServerIP[16], int Serverport)
{
    int sockfd = ssocket();
    sconnect(ServerIP, Serverport, sockfd);
    return sockfd;
}

void propositions()
{
    printf("To do a transfer: + [receiver] [amount]\n");
    printf("To do a recurring transfer: * [receiver] [amount]\n");
    printf("To quit: q\n");
}

void child_timer(void *d, void *p)
{
    int *delay = d;
    int *pipe = p;
    sclose(pipe[0]);

    while (1)
    {
        sleep(*delay);
        StructTransfer transfer;
        transfer.receiver = -1;
        swrite(pipe[1], &transfer, sizeof(transfer));
    }
    exit(EXIT_SUCCESS);
}

void child_recurrent_transfer(void *p, void *a, void *po)
{
    int *pipe = p;
    char *adr = a;
    int *port = po;

    sclose(pipe[1]);
    StructTransfer transfers[100];
    int sizeTransfers = 0;

    int nbRd;
    StructTransfer transfer;
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
    sclose(pipe[0]);
    exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[])
{
    if (argc != 5)
    {
        perror("Missing arguments\n");
        exit(0);
    }
    else
    {
        char adr[16];
        strcpy(adr, argv[1]);
        int port = atoi(argv[2]);
        int num = atoi(argv[3]);
        int delay = atoi(argv[4]);
        printf("Adresse: %s\n", adr);
        printf("Port: %d\n", port);
        printf("Num: %d\n", num);
        printf("Delay: %d\n", delay);

        StructTransfer transfer;
        transfer.sender = num;

        int pipe[2];

        spipe(pipe);

        int timerId = fork_and_run2(child_timer, &delay, &pipe);
        int recurrentTrasferId = fork_and_run3(child_recurrent_transfer, &pipe, &adr, &port);

        sclose(pipe[0]);
        propositions();

        char choice[CHOICE_SIZE];

        sread(0, choice, CHOICE_SIZE);

        while (choice[0] != 'q')
        {
            if (strlen(choice) > 1)
            {

                char *symbol = strtok(choice, " ");
                char *receiver = strtok(NULL, " ");
                char *amount = strtok(NULL, "\n");

                if (strcmp(symbol, "+") == 0)
                {

                    printf("+++ New transfer +++\n");

                    int sockfd = initSocketClient(adr, port);
                    transfer.receiver = atoi(receiver);
                    transfer.amount = atoi(amount);

                    StructMessage message;
                    message.transfers[0] = transfer;
                    message.sizeTransfers = 1;
                    swrite(sockfd, &message, sizeof(message));

                    // Message a ajouter
                    sread(sockfd, &message, sizeof(message));
                    if (message.code == TRANSFER_OK)
                    {
                        printf("The transfer has been done\n");
                        int newSold = message.newSolde;
                        printf("The new sold of your account is:%d\n", newSold);
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
                    printf("*** New recurrent transfer ***\n");
                    transfer.receiver = atoi(receiver);
                    transfer.amount = atoi(amount);
                    swrite(pipe[1], &transfer, sizeof(transfer));
                }
            }

            propositions();
            sread(0, choice, CHOICE_SIZE);
        }
        sclose(pipe[1]);
        skill(timerId, SIGKILL);
        skill(recurrentTrasferId, SIGKILL);
    }

    exit(EXIT_SUCCESS);
}