#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "utils_v1.h"
#include "config.h"

//******************************************************************************
// MAIN FUNCTION
//******************************************************************************
int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        perror("Missing arguments\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Getting semaphores
        int sem_id = sem_get(SEM_KEY, 1);
        // Getting the shared memory
        int shm_id = sshmget(SHM_KEY, 1000 * sizeof(int), 0);
        int *z = sshmat(shm_id);
        int account = atoi(argv[1]);
        int amount = atoi(argv[2]);
        printf("Current balance of your account: %d€\n", z[account]);

        // We decided to set a balance limit which is -500€ 
        // so the account's balance can't go below that limit
        if (z[account] + amount < LIMIT_AMOUNT)
        {
            perror("Overdraft balance\n");
        }
        else
        {
            sem_down0(sem_id);
            // Start of critical section
            z[account] += amount;
            // End of critical section
            sem_up0(sem_id);
            printf("Balance of your account after transaction: %d€\n", z[account]); 
        }
    }
    
    exit(EXIT_SUCCESS);
}
