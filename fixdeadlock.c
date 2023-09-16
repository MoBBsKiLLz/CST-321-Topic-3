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
    time_t t = time(NULL);
    struct tm date = *localtime(&t);
    
    sem_init(&semShared, 1, 1);

    pid_t pid = fork();

    FILE *fptr;

    if (pid == 0) {
        // Child Process
        fptr = fopen("log.txt", "a");
        t = time(NULL);
        date = *localtime(&t);
        fprintf(fptr, "[%d-%02d-%02d %02d:%02d:%02d] Child process is running.\n", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
        fclose(fptr);
        sem_wait(&semShared);
        fptr = fopen("log.txt", "a");
        t = time(NULL);
        date = *localtime(&t);
        fprintf(fptr, "[%d-%02d-%02d %02d:%02d:%02d] Semaphore acquired by Child.\n", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
        fclose(fptr);
        while(count < 60) {
            sleep(1);
            count++;
        }
        sem_post(&semShared);
    } else {
        // Parent Process
        fptr = fopen("log.txt", "a");
        t = time(NULL);
        date = *localtime(&t);
        fprintf(fptr, "[%d-%02d-%02d %02d:%02d:%02d] Parent process is running.\n", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
        fclose(fptr);
        pthread_t t1;
        int* res;
        pthread_create(&t1, NULL, &sleepyThread, NULL);
        pthread_join(t1, (void**) &res);
        if(*res == 1) {
            char ans;
            fptr = fopen("log.txt", "a");
            t = time(NULL);
            date = *localtime(&t);
            fprintf(fptr, "[%d-%02d-%02d %02d:%02d:%02d] Semaphore not available to Parent.\n", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
            fprintf(fptr, "[%d-%02d-%02d %02d:%02d:%02d] Parent is starved.\n", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
            fclose(fptr);
            printf("Would you like to kill the child process? ");
            scanf("%c", &ans);
            if (ans == 'y') {
                kill(pid, SIGKILL);
                int ppost = sem_post(&semShared);
            }
        }
        if (sem_wait(&semShared) == 0) {
            fptr = fopen("log.txt", "a");
            t = time(NULL);
            date = *localtime(&t);
            fprintf(fptr, "[%d-%02d-%02d %02d:%02d:%02d] Semaphore acquired by Parent\n", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
            fclose(fptr);
        }
        sem_post(&semShared);
        free(res);
    }
    sem_destroy(&semShared);
}