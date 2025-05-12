#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>
//检查线程错误时， 该函数无效，线程错误号是直接返回
void sys_err(const char* str){
    perror(str);//
    exit(1);
}

void *tfn(void* arg){
    printf("thread: pid = %d, tid = %lu\n", getpid(), pthread_self());
    return NULL;
}

int main(int argc, char *argv[]){
    pthread_t tid; 

    int ret = pthread_create(&tid, NULL, tfn, NULL);
    if(ret != 0){
        fprintf(stderr, "thread  create err: %s\n", strerror(ret));
        exit(1);
    }

    ret = pthread_detach(tid);
    if(ret != 0){
        fprintf(stderr, "thread  detach err: %s\n", strerror(ret));
        exit(1);
    }

    sleep(1);
    //detach的作用，设置分离后，线程终止自动清理pcb，无需回收
    ret = pthread_join(tid, NULL);
    printf("join ret = %d\n", ret);
    if(ret != 0){
        fprintf(stderr, "thread  join err: %s\n", strerror(ret));
        exit(1);
    }

    printf("main: pid = %d, tid = %lu\n", getpid(), pthread_self());

    pthread_exit((void *)0);
}