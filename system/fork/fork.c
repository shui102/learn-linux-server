#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char* argv[]){
    
    printf("before fork\n");
    printf("before fork 2\n");

    pid_t pid = fork();
    if(pid == -1){
        perror("fork error");
        exit(1);
    }
    else if(pid == 0){
        printf("child created \n");
    }
    else if(pid > 0){
        printf("child pid = %d\n", pid);
    }

    printf("end of file\n");
    
    return 0;
}