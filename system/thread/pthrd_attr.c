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
    pthread_attr_t attr;
    int ret = pthread_attr_init(&attr);
    if(ret != 0){
        fprintf(stderr, "attr  create err %s\n", strerror(ret));
        exit(1);
    }
    //设置线程属性为分离
    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if(ret != 0){
        fprintf(stderr, "attr  set err %s\n", strerror(ret));
        exit(1);
    }

    ret = pthread_create(&tid, &attr, tfn, NULL);
    if(ret != 0){
        fprintf(stderr, "thread  create err %s\n", strerror(ret));
        exit(1);
    }

    ret = pthread_attr_destroy(&attr);
    if(ret != 0){
        fprintf(stderr, "destroy  create err %s\n", strerror(ret));
        exit(1);
    }
    
    printf("main: pid = %d, tid = %lu\n", getpid(), pthread_self());
    return 0;
}