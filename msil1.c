#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#define N 10

int buffer[N];
int writer = 0;
int reader = 0;

int put(int item) {
    if ((writer + 1) % N == reader) {
        printf("Buffer is full!");
        return 0;
    }
    buffer[writer] = item;
    writer = (writer + 1) % N;
    return 1;
}

int get(int* value) {
    if (reader == writer) {
        printf("Buffer is empty!");
        return 0;
    }
    *value = buffer[reader];
    reader = (reader + 1) % N;
    printf("Got letter %c\n", *value);
    return 1;
}

void read_message() {

}

void main(){
    int pid = fork();
    if (pid == -1) {
        printf("Fork failed.\n");
    }

    if (pid == 0) {
        printf("Child process created.\n");
        signal(SIGUSR1, read_message);
    } else {
        printf("Parent process created.\n");
        char message[] = "Miguel";
        int length = sizeof(message) / sizeof(message[0]);
        for (int i = 0; i < length; i++) {
            put(message[i]);
            if(i == 1) {
                kill(pid, SIGUSR1);
            }
        }
    }
}