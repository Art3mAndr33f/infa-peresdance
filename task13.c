#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/syscall.h>
#include <poll.h>

int main(int argc, char* argv[]){
    if (argc != 2){
        printf("Incorrect argc: %d. Correct is 2\n", argc);
        exit(-1);
    }
    if ((strcmp(argv[1], "a") != 0) && (strcmp(argv[1], "b") != 0)){
        printf("Incorrect argv[1]. Correct: (a|b).\n");
        exit(-1);
    }
    pid_t pid = fork();
    if (pid == -1){
        printf("Fork error\n");
        exit(-1);
    }
    if (strcmp(argv[1], "a") == 0){
        if (pid == 0){
            int chpid = getpid();
            printf("Child (PID: %d) is started\n", chpid);
            printf("Child is sleep..\n");
            sleep(4);
            exit(-1);
        } else {
            int status = 0;
            int ppid = getpid();
            printf("Parent (PID: %d) is started\n", ppid);
            if (waitpid(pid, &status, WUNTRACED | WCONTINUED) < 0){
                printf("waitpid error.\n");
                exit(-1);
            }
            while (WIFEXITED(status) == 0){
                if (WIFCONTINUED(status)){
                    printf("Child is sontinue to work\n");
                }
                if (waitpid(pid, &status, WUNTRACED | WCONTINUED) < 0){
                    printf("waitpid error.\n");
                    exit(-1);
                }
            }
            if (WIFEXITED(status)){
                printf("Child process was exited with exit status %d\n", WEXITSTATUS(status));
            }
            printf("Parent was exited.\n");
            exit(-1);
        }
    } else if (strcmp(argv[1], "b") == 0){
        if (pid == 0){
            int chpid = getpid();
            printf("Child (PID: %d) is started\n", chpid);
            struct pollfd poll0;
            poll0.fd = getppid();
            poll0.events = POLLIN;
            printf("Wait for parent..\n");
            int ready = poll(&poll0, 1, -1);
            if (ready < 0){
                printf("Poll error\n");
                exit(-1);
            }
            printf("Child was exited\n");
            exit(-1);
        } else {
            int ppid = getpid();
            printf("Parent (PID: %d) is started\n", ppid);
            printf("Parent is sleep..\n");
            sleep(4);
            printf("Parent was exited\n");
            exit(-1);
        }
    } else {
        printf("Incorrect argv[1]. Correct: (a|b).\n");
        exit(-1);
    }
    return 0;
}
