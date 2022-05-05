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

void child_timer(int delay)
{
    while (1)
    {

        sleep(delay);
    }
}

void child_transfer_()
{
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

        structTransfer transfer;
        transfer.sender = num;

        propositions();

        char choice[CHOICE_SIZE];

        sread(0, choice, CHOICE_SIZE);
        choice[strlen(choice) - 1] = '\0';

        while (choice[0] != 'q')
        {
            if (strlen(choice) > 1)
            {

                char dividedChoise[4][50];
                int j = 0, ctr = 0;
                for (int i = 0; i < strlen(choice); i++)
                {
                    if (choice[i] == ' ' || choice[i] == '\0')
                    {
                        dividedChoise[ctr][j] = '\0';
                        ctr++;
                        j = 0;
                    }
                    else
                    {
                        dividedChoise[ctr][j] = choice[i];
                        j++;
                    }
                }

                if (choice[0] == '+')
                {
                    printf("type:%s|\n", dividedChoise[0]);
                    printf("compte:%s|\n", dividedChoise[1]);
                    printf("montant:%s|\n", dividedChoise[2]);

                    int sockfd = initSocketClient(adr, port);
                    transfer.receiver = atoi(dividedChoise[1]);
                    transfer.amount = atoi(dividedChoise[2]);
                    swrite(sockfd, &transfer, sizeof(transfer));

                    // Message a ajouter

                    sclose(sockfd);
                }
                else if (choice[0] == '*')
                {
                    printf("type:%s|\n", dividedChoise[0]);
                    printf("compte:%s|\n", dividedChoise[1]);
                    printf("montant:%s|\n", dividedChoise[2]);
                }
            }

            propositions();
            sread(0, choice, CHOICE_SIZE);
            choice[strlen(choice) - 1] = '\0';
        }
    }

    return 0;
}