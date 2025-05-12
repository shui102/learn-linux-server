#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
    struct stat sb;

    int ret = stat(argv[1], &sb);
    if(ret == -1){
        perror("stat error");
        exit(1);
    }
    
    if(S_ISREG(sb.st_mode)){
        printf("It is regular\n");
    }else if(S_ISDIR(sb.st_mode)){
        printf("It is Dir\n");
    }else if(S_ISFIFO(sb.st_mode)){
        printf("It is FIFO\n");
    }else{
        printf("No\n");
    }

    return 0;
}