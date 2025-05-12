#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>

void sys_err(const char* str){
    perror(str);
    exit(1);
}

void *tfn(void* arg){
    printf("thread: pid = %d, tid = %lu\n", getpid(), pthread_self());
    return NULL;
}

int main(int argc, char *argv[]){
    pthread_t tid;

    printf("main: pid = %d, tid = %lu\n", getpid(), pthread_self());

    int ret = pthread_create(&tid, NULL, tfn, NULL);
    if(ret != 0){
        fprintf(stderr, "thread  create err %s\n", strerror(ret));
    }
    sleep(1);
    return 0;
}