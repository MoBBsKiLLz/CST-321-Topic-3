#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define N 10

char buffer[N];
int writeIndx = 0;
int readIndx = 0;
int count = 0;

int ParentProcess(char letter) {

    if(count == N - 1) {
        return 0; // Buffer is full
    }

    buffer[writeIndx] = letter;
    writeIndx = (writeIndx + 1)  % N;
    count++;
    
    return 1;
}

int ChildProcess(char* letter) {
    if (readIndx == writeIndx) {
        return 0; // Buffer is empty
    }

    *letter = buffer[readIndx];
    readIndx = (readIndx + 1) % N;
    count--;

    printf("Got letter %c\n", *letter);
    return 1;
}

void charavail(int signum) {
    int size = count;
    for(int i = 0; i < size; i++) {
        ChildProcess(&buffer[i]);
        sleep(1);
    }
}

void complete(int signum) {
    exit(1);
}

void main() {
    signal(SIGUSR1, charavail);
    signal(SIGUSR2, complete);
    pid_t pid = fork();

    if (pid == 0) {
        wait(NULL);
    } 
    
    char word[] = "Miguel";
    int length = sizeof(word) / sizeof(word[0]);
    for (int i = 0; i < length; i++) {
        ParentProcess(word[i]);

        if (i == 1) {
            kill(pid, SIGUSR1);
            printf("Character available.\n");
        } else if (i == length - 1) {
            ParentProcess('\0');
            kill(pid, SIGUSR2);
            printf("End of string.\n");
        }

        sleep(1);
    }

    exit(1);     
}