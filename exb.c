#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int main(void){
    int *array;
    int shmid;
    int new = 1;
    char pathname[] = "exa.c";
    key_t key;
    if((key = ftok(pathname,0)) < 0){
        printf("Can\'t generate key\n");
        exit(-1);
    }
    if((shmid = shmget(key, 3*sizeof(int), 0666|IPC_CREAT|IPC_EXCL)) < 0){
        if(errno != EEXIST){
            printf("Can\'t create shared memory\n");
            exit(-1);
        } else {
            if((shmid = shmget(key, 3*sizeof(int), 0)) < 0){
                printf("Can\'t find shared memory\n");
                exit(-1);
            }
            new = 0;
        }
    }
    if((array = (int *)shmat(shmid, NULL, 0)) == (int *)(-1)){
        printf("Can\'t attach shared memory\n");
        exit(-1);
    }
    if(new){
        array[0] = 0;
        array[1] = 1;
        array[2] = 1;
    } else {
        array[1] += 1;
        array[2] += 1;
    }
    printf("            | prog1 | prog2 | total\n");
    printf("spawn times |%6d |%6d |%6d\n", array[0], array[1], array[2]);
    if(shmdt(array) < 0){
        printf("Can\'t detach shared memory\n");
        exit(-1);
    }
    printf("To remove shared memory write:\nipcrm shm %d\n", shmid);
    return 0;
}