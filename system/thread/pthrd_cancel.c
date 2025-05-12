#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>

struct thrd{
    int var;
    char str[256];
};

void sys_err(const char* str){
    perror(str);
    exit(1);
}

void *tfn(void* arg){
    while(1){
        printf("thread pid = %d, tid = %lu\n", getpid(), pthread_self());
    }
    return NULL;
}

int main(int argc, char *argv[]){
    pthread_t tid;

    int ret = pthread_create(&tid, NULL, tfn, NULL);
    if(ret != 0){
        perror("create err");
    }
    printf("main pid = %d. tid = %lu\n", getpid(), pthread_self());
    sleep(5);
    ret = pthread_cancel(tid); //终止线程
    if(ret != 0){
        perror("cancel err");
    }

    while(1);
    pthread_exit((void *)0);
}