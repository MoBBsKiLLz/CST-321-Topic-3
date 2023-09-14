#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define N 10


struct CIRCULAR_BUFFER {
    int buffer[N];
    int writer;
    int reader;
    int count;
};
struct CIRCULAR_BUFFER *buffer = NULL;

int put(int item) {
    if ((buffer->writer + 1) % N == buffer->reader) {
        return 0; // Buffer is full
    }
    buffer->buffer[buffer->writer] = item;
    buffer->writer = (buffer->writer + 1) % N;
    return 1;
}

int get(int* value) {
    if (buffer->reader == buffer->writer) {
        return 0; // Buffer is empty
    }
    *value = buffer->buffer[buffer->reader];
    buffer->reader = (buffer->reader + 1) % N;
    printf("Got letter %c\n", *value);
    return 1;
}

void readBufferMessage() {
    int i = 0;
    while(get(&buffer->buffer[i])){
        i++;
    }
}

void handle_sigusr1(int signum) {
    readBufferMessage();
}

void handle_sigusr2(int signum) {
    exit(0);
}

void main(){
    buffer = (struct CIRCULAR_BUFFER*)mmap(0, sizeof(buffer), PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    buffer->count = 0;
    buffer->reader = 0;
    buffer->writer = 0;

    signal(SIGUSR1, handle_sigusr1);
    signal(SIGUSR2, handle_sigusr2);

    int pid = fork();
    if (pid == -1) {
        printf("Fork failed.\n");
    }

    if (pid == 0) {
        while(1);
        exit(1);
    } else {
        char message[] = "Miguel";
        int length = sizeof(message) / sizeof(message[0]);
        for (int i = 0; i < length; i++) {
            put(message[i]);
            if(i == 1) {
                kill(pid, SIGUSR1);
            }
            if (i == length - 1) {
                sleep(1);
                kill(pid, SIGUSR2);
            }
        }
        exit(1);
    }
}