#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

void sys_err(const char* str){
    perror(str);
    exit(1);
}

int main(int argc, char *argv[]){
    pid_t pid;
    int ret;

    pid = fork();
    if(pid > 0) exit(0);

    pid = setsid();
    if(pid == -1){
        sys_err("setsid err");
    }

    chdir("/home/shui/learn");
    if(ret == -1)
        sys_err("chdir err");

    umask(0022);
    close(STDIN_FILENO);
    int fd = open("/dev/null", O_RDWR);
    if(fd == -1) sys_err("open err");

    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    
    while(1);

    return 0;
}