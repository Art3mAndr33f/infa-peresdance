#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(void){
    int fd, pid;
    size_t size;
    char str[7];
    char name[] = "a.fifo";
    (void) umask(0);
    if (mknod(name, S_IFIFO | 0666, 0) < 0) {
        printf("Can\'t creat FIFO. \"rm a.fifo\" if it is in the dir.\n");
        exit(-1);
    }
    if ((pid = fork()) < 0) {
        printf("Can\'t fork child\n");
        exit(-1);
    } else if (pid > 0) {
        if ((fd = open(name, O_WRONLY)) < 0){
            printf("Can\'topen FIFO for writing\n");
            exit(-1);
        }
        size = write(fd, "Hello!", 7);
        if(size != 7){
            printf("Can\'t write all string to FIFO\n");
            exit(-1);
        }
        close(fd);
        printf("Parent exit\n");
    } else {
        if((fd = open(name, O_RDONLY)) < 0){
            printf("Can\'t open FIFO for reading\n");
            exit(-1);
        }
        size = read(fd, str, 7);
        if((int)size < 0){
            printf("Can\'t read string\n");
            exit(-1);
        }
        printf("%s\n",str);
        close(fd);
        printf("Child exit\n");
    }
    return 0;
}
