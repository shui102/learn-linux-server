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
    int i = (int)arg;
    sleep(i);
    printf("%dth thread: pid = %d, tid = %lu\n", i + 1, getpid(), pthread_self());
    return NULL;
}

int main(int argc, char *argv[]){
    int i;
    int ret;
    pthread_t tid;
    for(i = 0; i< 5; i++){
        ret = pthread_create(&tid, NULL, tfn, (void *)i);
        if(ret != 0){
            perror("pthrd create err");
        }
    }
    sleep(i);
    return 0;
}