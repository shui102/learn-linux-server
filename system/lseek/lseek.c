#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
    char* msg = "Test for lseek\n";
    int n;
    char ch;
    int fd = open("lseek.txt", O_RDWR | O_CREAT | O_TRUNC, 0664);
    write(fd, msg, strlen(msg));

    lseek(fd, 0 , SEEK_SET);

    while((n = read(fd, &ch, 1))){
        if(n < 0){
            perror("read error");
            exit(1);
        }
        write(STDOUT_FILENO, &ch, n);
    }
    close(fd);
    return 0;
}