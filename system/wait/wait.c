#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
int main(int argc, char* argv[]){
    
    pid_t pid, wpid;
    int status;
    pid = fork();
    if(pid == 0){
        printf("--- child, my id = %d, sleep 10s\n", getpid());
        sleep(10);
        printf("---child died---\n");
        return 73;
    }
    else if(pid > 0){
        wpid = wait(&status);
        if(wpid == -1){
            perror("wait error");
            exit(1);
        }
        if(WIFEXITED(status)){
            printf("child exit with return %d\n", WEXITSTATUS(status));
        }
        printf("parent wait finished\n");
    }
    else{
        perror("fork");
        return 1;
    }
    return 0;
}