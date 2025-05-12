#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#define SERV_PORT 6666
#define MAXSIZE 10

void sys_err(const char* str){
    perror(str);
    exit(1);
}

int main(int argc, char* agrv[]){
    int lfd = 0, connfd = 0, efd = 0;
    int flag;
    int n, i;
    char buf[BUFSIZ], client_IP[1024], str[INET_ADDRSTRLEN];

    struct sockaddr_in serv_addr, clit_addr;
    socklen_t clit_addr_len;
    clit_addr_len = sizeof(clit_addr);

    //memset(&serv_addr, 0, sizeof(serv_addr));
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    lfd = socket(AF_INET, SOCK_STREAM, 0);
    
    //设置进程端口复用
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    
    listen(lfd, 128);

    struct epoll_event event;
    struct epoll_event res_event[10];
    int res,len;

    efd = epoll_create(10);
    event.events = EPOLLIN | EPOLLET; //ET边沿触发

    printf("Accepting connections...\n");
    clit_addr_len = sizeof(clit_addr);
    connfd = accept(lfd, (struct sockaddr *)&clit_addr, &clit_addr_len);

    printf("received from %s at PORT %d\n", 
        inet_ntop(AF_INET, &clit_addr.sin_addr, str, sizeof(str)),
        ntohs(clit_addr.sin_port)  
        );
    
    //设置connfd为非阻塞读
    flag = fcntl(connfd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(connfd, F_SETFL, flag);

    event.data.fd = connfd;
    epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &event);
    while(1){
        printf("epoll wait begin\n");
        res = epoll_wait(efd, res_event, 10, -1);  //最多10个，阻塞监听
        printf("epoll wait end res %d\n", res);

        if(res_event[0].data.fd == connfd){
            while((len = read(connfd, buf, MAXSIZE/2)) > 0){
                write(STDOUT_FILENO, buf, len);
            }
        }
    }
    
    return 0;
}