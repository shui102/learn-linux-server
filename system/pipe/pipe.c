#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char* argv[]){
    int fd[2];
    int ret;
    pid_t pid;
    char *str = "hello pipe\n";
    char buf[1024];


    ret = pipe(fd);
    if(ret == -1){
        perror("pipe error");
    }
    pid = fork();
    if(pid > 0){
        close(fd[0]);
        write(fd[1], str, strlen(str));
        sleep(1);
        close(fd[1]);
    }else if(pid == 0){
        close(fd[1]);
        ret = read(fd[0], buf, sizeof(buf));
        write(STDOUT_FILENO, buf ,ret);
        close(fd[0]);
    }
    return 0;
}