#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

void sys_err(const char* str){
    perror(str);
    exit(1);
}

int main(int argc, char* argv[]){
    int ret;
    int fd[2];
    pid_t pid;
    int i;

    ret = pipe(fd);
    if(ret == -1){
        sys_err("pipe error");
    }

    for(i = 0; i < 2; i++){
        pid = fork();
        if(pid == 0){
            break;
        }    
        if(pid == -1){
            sys_err("fork error");
        }
    }

    if(i == 2){
        close(fd[0]);
        close(fd[1]);
        wait(NULL);
        wait(NULL);
    }else if(i == 0){ 
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        execlp("ls", "ls", NULL);

    }else if(i == 1){
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        execlp("wc", "wc", "-l", NULL);
    }

    return 0;
}