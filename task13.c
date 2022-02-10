#define _GNU_SOURCE 
#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <errno.h>  
#include <sys/wait.h>
#include <sys/stat.h> 
#include <stdlib.h> 
#include <signal.h>
#include <string.h>

int main(void) {
    pid_t child_id = fork(); 
    if(child_id < 0) {
        perror("fork");
        return 1;
    }  

    if(child_id == 0) {
        pid_t child = getpid(); 
        printf("Child process PID = %d\n", child);
        sleep(60);
    } else {
        int status = 0; 
        /*WUNTRACED   also  return  if  a child has stopped (but not traced via ptrace(2)).  
        Status for traced children which have stopped is provided even if this option is not specified.*/
        /*WCONTINUED (since Linux 2.6.10)
        also return if a stopped child has been resumed by delivery of SIGCONT.*/
        if(waitpid(child_id, &status, WUNTRACED | WCONTINUED ) < 0) {
            perror("waitpid"); 
            return 2; 
        }  
        //WIFEXITED(wstatus) returns true if the child terminated normally
        //WIFSIGNALED(wstatus) returns true if the child process was terminated by a signal.
        while(WIFEXITED(status) == 0 && WIFSIGNALED(status) == 0) {
            // WIFSTOPPED(wstatus) returns true if the child process was stopped by delivery of a signal
            if(WIFSTOPPED(status)) {
                /*WSTOPSIG(wstatus)
                returns the number of the signal which caused the child to stop.
                WTERMSIG(wstatus)
                returns  the  number  of  the  signal that caused the child process to terminate.*/

                /*const char *sigdescr_np(int sig)
                The sigdescr_np() function returns a string describing the signal
                number passed in the argument sig
                */

                printf("Child with PID = %d was stopped by signal %d (%s)\n", child_id, WSTOPSIG(status), sigdescr_np(WTERMSIG(status)));     
            } 
            // WIFCONTINUED(wstatus) (since Linux 2.6.10) returns true if the child process was resumed by delivery of SIGCONT.
            if(WIFCONTINUED(status)) {
                printf("Child with PID = %d continue to work\n", child_id);
            } 
            if(waitpid(child_id, &status, WUNTRACED | WCONTINUED) < 0) {
                perror("waitpid"); 
                return 3;
            }
        }
        if(WIFEXITED(status)) { 
            printf("Child process with PID = %d exited with code %d\n", child_id, WEXITSTATUS(status));
        } 
        else if(WIFSIGNALED(status)) {
            /*WTERMSIG(wstatus)
            returns  the  number  of  the  signal that caused the child process to terminate.*/
            int child_signal = WTERMSIG(status);
            printf("Child with PID = %d was killed by signal %d (%s)\n",  child_id, child_signal, sigdescr_np(child_signal));
            /*WCOREDUMP(wstatus)
            returns true if the child produced a core dump (see core(5)).  
            This macro should be employed only if WIFSIGNALED returned true.*/
            if(WCOREDUMP(status)) {
                printf("core dump\n");
            }
        }
    }
    return 0;
}