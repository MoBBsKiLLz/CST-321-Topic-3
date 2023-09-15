#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>

sem_t semShared;
int count = 0;

void* sleepyThread() {
    int fail = 1;
    int pass = 0;
    int* resultFail = malloc(sizeof(int));
    int* resultPass = malloc(sizeof(int));
    *resultFail = fail;
    *resultPass = pass;
    sleep(10);
    if (sem_wait(&semShared) == 0) {
        return (void*) resultFail;
    }
    return (void*) resultPass;
}

void main(){
    sem_init(&semShared, 1, 1);

    FILE *fptr = fopen("log.txt", "w");
    if (fptr == NULL) {
        printf("Could not open file");
    }

    pid_t pid = fork();

    if (pid == 0) {
        // Child Process
        fprintf(fptr, "Child process is running.\n");
        sem_wait(&semShared);
        while(count < 60) {
            sleep(1);
            count++;
        }
        sem_post(&semShared);
    } else {
        // Parent Process
        pthread_t t1;
        int* res;
        // The threads routine can instead prompt the user to check the file for the availability of the semaphore.
        pthread_create(&t1, NULL, &sleepyThread, NULL);
        pthread_join(t1, (void**) &res);
        if(*res == 1) {
            kill(pid, SIGKILL);
            int ppost = sem_post(&semShared);
        }
        if (sem_wait(&semShared) == 0) {
            printf("...Semaphore acquired by Parent\n");
        }
        sem_post(&semShared);
        free(res);
    }
    sem_destroy(&semShared);
}