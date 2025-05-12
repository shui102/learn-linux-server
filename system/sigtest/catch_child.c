#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>


void catch_child(int signo){
    pid_t wpid;
    int status;
    while((wpid = waitpid(-1, &status, 0)) != -1){
        if(WIFEXITED(status)){
            printf("---------catch child id = %d, ret = %d\n", wpid, WEXITSTATUS(status));
        }
    }
    return;
}

void sys_err(const char* str){
    perror(str);
    exit(1);
}

int main(int argc, char *argv[]){
    pid_t pid;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_BLOCK, &set, NULL);

    int i;
    for(i = 0; i < 15; i++){
        if((pid = fork()) == 0)break;
    }
    if(15 == i){
        struct sigaction act;
        act.sa_handler = catch_child;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;

        sigaction(SIGCHLD, &act, NULL);

        //解除阻塞
        sigprocmask(SIG_UNBLOCK, &set, NULL);

        printf("I am parent, pid = %d\n", getpid());

        while(1);
    }else{
        printf("I am child, pid = %d\n", getpid());
        return i;
    }

    return 0;
}