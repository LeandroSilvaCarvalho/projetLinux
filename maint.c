#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "utils_v1.h"
#include "config.h"

void checkUsage(int argc, char *argv[])
{
    if (argc != 2 || (!strcmp(argv[1], "1") && !strcmp(argv[1], "2")))
    {
        printf("Usage :\n");
        printf("%s 1 to create IPCs\n", argv[0]);
        printf("%s 2 to destroy IPCs\n", argv[0]);
        printf("%s 3 [opt] to reserve exclusively the shared account book for [opt] seconds\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    checkUsage(argc, argv);

    if (!strcmp(argv[1], "1"))
    {
        // IPC creation and initialization
        int shm_id = shmget(SHM_KEY, 2 * sizeof(pid_t), IPC_CREAT | IPC_EXCL | PERM);
        checkNeg(shm_id, "IPCs already created");

        sem_create(SEM_KEY, 1, PERM, 1);

        printf("IPCs created\n");
    }
    else if (!strcmp(argv[1], "2"))
    {
        // IPC destruction
        printf("Destroying IPCs...\n");
        int shm_id = shmget(SHM_KEY, 2 * sizeof(pid_t), 0);
        checkNeg(shm_id, "IPCs not existing");

        sshmdelete(shm_id);

        int sem_id = sem_get(SEM_KEY, 1);
        sem_delete(sem_id);

        printf("IPCs freed\n");
    }

    exit(EXIT_SUCCESS);
}
