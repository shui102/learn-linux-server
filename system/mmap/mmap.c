#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>

void sys_err(const char* str){
    perror(str);
    exit(1);
}

int main(int argc, char *argv[]){
    char* p;
    int fd;
    fd = open("testmap",O_RDWR | O_CREAT | O_TRUNC , 0664);
    if(fd == -1){
        sys_err("open err");
    }
/*
    lseek(fd, 10, SEEK_END); //等价于truncate 拓展文件大小
    write(fd, "\0", 1);
*/
    
    ftruncate(fd, 20);
    int len = lseek(fd, 0, SEEK_END);
    p = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if(p == MAP_FAILED){
        sys_err("mmap err");
    }

    //使用p对文件进行读写操作
    strcpy(p, "hello mmap");
    printf("---------%s----------\n", p);

    int ret = munmap(p, len);
    if(ret == -1){
        sys_err("unmap err");
    }
    return 0;
}