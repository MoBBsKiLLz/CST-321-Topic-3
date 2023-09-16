#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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
    printf("Check log file.\n");
    if (sem_wait(&semShared) == 0) {
        return (void*) resultFail;
    }
    return (void*) resultPass;
}

void main(){
    sem_init(&semShared, 1, 1);

    pid_t pid = fork();

    FILE *fptr = fopen("log.txt", "w");

    if (pid == 0) {
        // Child Process
        fprintf(fptr, "Child process is running.\n");
        sem_wait(&semShared);
        fprintf(fptr, "Semaphore acquired by Child.\n");
        while(count < 60) {
            sleep(1);
            fprintf(fptr, "Waiting in child process for %d second/s.\n", count + 1);
            count++;
        }
        sem_post(&semShared);
    } else {
        // Parent Process
        fprintf(fptr, "Parent process is running.\n");
        pthread_t t1;
        int* res;
        pthread_create(&t1, NULL, &sleepyThread, NULL);
        pthread_join(t1, (void**) &res);
        if(*res == 1) {
            char ans;
            fprintf(fptr, "Semahore not available to Parent.\n");
            fprintf(fptr, "Parent is starved.\n");
            fclose(fptr);
            printf("Would you like to kill the child process? ");
            scanf("%c", &ans);
            if (ans == 'y') {
                kill(pid, SIGKILL);
                int ppost = sem_post(&semShared);
            }
        }
        if (sem_wait(&semShared) == 0) {
            printf("Semaphore acquired by Parent\n");
        }
        sem_post(&semShared);
        free(res);
    }
    sem_destroy(&semShared);
}