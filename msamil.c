#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_NUM 2

int count = 0;
int finalCount = 0;

pthread_mutex_t countMutex;

void* counter(void* arg) {
    int seconds = (*(int*)arg) * 60;
    for (int i = 0; i < seconds; i++) {
        sleep(1);
        pthread_mutex_lock(&countMutex);
        count++;
        pthread_mutex_unlock(&countMutex);
    }
    printf("The count for the counter is: %d\n", count);
    finalCount = count;
}

void* monitor(void* arg) {
    int seconds = (*(int*)arg) * 60;
    int misses = 0;
    for (int i = 0; i < seconds; i+=3) {
        pthread_mutex_trylock(&countMutex);
        misses++;
        printf("The count for the monitor is: %d\n", count);
        pthread_mutex_unlock(&countMutex);
        sleep(3);
        if (finalCount == count) {
            break;
        }
    }
    printf("Number of misses: %d\n", misses);
}

void main(){
    pthread_t th[THREAD_NUM];
    pthread_mutex_init(&countMutex, NULL);
    int minutes1 = 1;
    int minutes2 = 2;
    int i;
    for(i = 0; i < THREAD_NUM; i++) {
        if(i % 2 == 0) {
            if(pthread_create(&th[i], NULL, &counter, &minutes1) != 0) {
                perror("Failed to create thread");
            }
        } else {
            if (pthread_create(&th[i], NULL, &monitor, &minutes2) != 0) {
                perror("Failed to create thread");
            }
        }
    }
    for(i = 0; i < THREAD_NUM; i++) {
        if(pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
    pthread_mutex_destroy(&countMutex);
}