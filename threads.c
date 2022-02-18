#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int a = 0;

void *mythread(void *dummy){
    dummy = dummy;
    pthread_t mythid;
    mythid = pthread_self();
    a = a + 1;
    printf("Thread %ld, Calculation result = %d\n", mythid, a);
    return 0;
}

int main(void){
    pthread_t thid, mythid;
    int result;
    int pid = fork();
    if(pid < 0){
        printf("Fork error\n");
        exit(-1);
    } 
    if(pid == 0){
        result = pthread_create( &thid, (pthread_attr_t *)NULL, mythread, NULL);
        if(result != 0){
            printf("Error on thread1 create, return value: %d\n", result);
            exit(-1);
        }
        printf("Thread created in child process, with thid: %ld\n", thid);
    } else {
        result = pthread_create( &thid, (pthread_attr_t *)NULL, mythread, NULL);
        if(result != 0){
            printf("Error on thread2 create, return value: %d\n", result);
            exit(-1);
        }
        printf("Thread created in parent process, with thid: %ld\n", thid);
    }
    mythid = pthread_self();
    a = a + 1;
    printf("Thread %ld, Calcuation result = %d\n", mythid, a);
    pthread_join(thid, (void **)NULL);
    return 0;
}
