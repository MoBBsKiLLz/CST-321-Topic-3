#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
void main() {
    FILE *fptr = fopen("sample.txt", "w");
    if (fptr == NULL) {
        printf("Could not open file");
    }

    pid_t pid = fork();
    if (pid == 0) {
        fprintf(fptr, "Child process running.\n");
    } else {
        fprintf(fptr, "Parent process running.\n");
    }
    fclose(fptr);
}