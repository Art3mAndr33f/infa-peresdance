#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char* argv[]){
    if (argc != 2){
        printf("Incorrect argc: %d. Correct is 2\n", argc);
        exit(-1);
    }
    int fldpr = open("text.txt", O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(fldpr < 0){
        printf("Open error\n");
        exit(-1);
    }
    if(dup2(fldpr, fileno(stdout)) < 0){
        printf("Dup2 error\n");
        close(fldpr);
        exit(-1);
    }
    printf("%s\n", argv[1]);
    close(fldpr);
    return 0;
}
