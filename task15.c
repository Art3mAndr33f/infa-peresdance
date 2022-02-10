#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int g;

void handler(int sig) {
    g = sig;
}

int main(void)
{   
    pid_t inf = getpid(); 
    printf("PID of Procces %d\n", inf);
    struct sigaction universal_sa = {};
    universal_sa.sa_handler = &handler;
    sigemptyset(&universal_sa.sa_mask);
    for (int sig_num = 1; sig_num <= NSIG; sig_num++)
        if (sigaction(sig_num, &universal_sa, NULL)) {
            perror("sigaction");
            psignal(sig_num, NULL);
        }

    for(;;){ 
        pause();
    }
    return 0;
}