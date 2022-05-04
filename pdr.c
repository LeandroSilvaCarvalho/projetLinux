#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "utils_v1.h"
#include "config.h"

int main(int argc, char const *argv[])
{

    if (argc < 3)
    {
        perror("Missing arguments\n");
        exit(0);
    }
    else
    {
        int sem_id = sem_get(SEM_KEY, 1);
        int shm_id = sshmget(SHM_KEY, 1000 * sizeof(int), 0);
        int *z = sshmat(shm_id);
        int account = atoi(argv[1]);
        int amount = atoi(argv[2]);

        printf("Amount of the account: %d\n", z[account]);
        if (z[account] + amount < LIMIT_AMOUNT)
        {
            perror("Overdraft amount\n");
        }
        else
        {
            sem_down0(sem_id);
            z[account] += amount;
            sem_up0(sem_id);
            printf("Amount of the account after transaction: %d\n", z[account]); 
        }
    }
    return 0;
}
