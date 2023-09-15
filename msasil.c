#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>

sem_t semShared;

void* sleepyThread() {
    printf("Sleepy thread will wait for 10 sec\n");
    sleep(10);
    sem_wait(&semShared);
}

void main(){
    sem_init(&semShared, 1, 1);

    pid_t pid = fork();

    if (pid == 0) {
        // Child Process
        sem_wait(&semShared);
        int i = 0;
        while(i < 60) {
            sleep(1);
            printf("Waiting in child process for %d second/s.\n", i + 1);
            i++;
        }
        sem_post(&semShared);
    } else {
        // Parent Process
        pthread_t t1;
        void* result;
        printf("Starting thread.\n");
        pthread_create(&t1, NULL, &sleepyThread, NULL);
        pthread_join(t1, &result);
        printf("Thread is done.\n");
        printf("Killing child process.\n");
        kill(pid, SIGKILL);
        sem_post(&semShared);
        sem_wait(&semShared);
        printf("Semaphore acquired by Parent\n");
        sem_post(&semShared);
    }
    sem_destroy(&semShared);
}