#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

struct student
{
    int id;
    char name[256];
    int age;
};


void sys_err(const char* str){
    perror(str);
    exit(1);
}

int main(int argc, char *argv[]){
    struct student stu = {10, "xiaoming", 18};
    struct student *p;
    int fd;
    fd = open("test map", O_RDWR | O_CREAT | O_TRUNC, 0664);
    if(fd == -1){
        sys_err("open err");
    }

    ftruncate(fd, sizeof(stu));

    p = mmap(NULL, sizeof(stu), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(p == MAP_FAILED){
        sys_err("mmap err");
    }
    while(1){
        memcpy(p, &stu, sizeof(stu));
        stu.id++;
        sleep(1);
    }

    munmap(p, sizeof(stu));
    close(fd);
    return 0;
}