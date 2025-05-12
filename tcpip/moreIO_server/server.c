#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERV_PORT 6666


void sys_err(const char* str){
    perror(str);
    exit(1);
}

int main(int argc, char* agrv[]){
    int lfd = 0, maxfd = 0, connfd = 0, sockfd = 0;
    int nready, client[FD_SETSIZE];
    int n, maxi, i;
    pid_t pid;
    int ret;
    char buf[BUFSIZ], client_IP[1024], str[INET_ADDRSTRLEN];

    maxi = -1;
    for(int i = 0; i < FD_SETSIZE; i++){
        client[i] = -1;
    }

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

    fd_set rset, allset;
    maxfd = lfd;

    FD_ZERO(&allset);
    FD_SET(lfd, &allset);

    while(1){
        rset = allset;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

        if(nready < 0){
            sys_err("select err");
        }

        if(FD_ISSET(lfd, &rset)){
            clit_addr_len = sizeof(clit_addr);
            connfd = accept(lfd, (struct sockaddr *)&clit_addr, &clit_addr_len);
            printf("received from %s at PORT %d\n", 
            inet_ntop(AF_INET, &clit_addr.sin_addr, str, sizeof(str)),
            ntohs(clit_addr.sin_port)  
            );

            for(i = 0; i < FD_SETSIZE; i++){
                if(client[i] < 0){
                    client[i] = connfd;
                    break;
                }
            }
            
            if(i == FD_SETSIZE){
                fputs("too many clients\n", stderr);
                exit(1);
            }

            FD_SET(connfd, &allset);
            if(maxfd < connfd)maxfd = connfd;

             if(i > maxi) maxi = i;
            
            if(0 == --nready)continue;  //只有lfd执行
        }
        
        for(i = 0; i <= maxi; i++){
            if((sockfd = client[i]) < 0){
                continue;
            }
            if(FD_ISSET(sockfd, &rset)){
                if((n = read(sockfd, buf, sizeof(buf))) == 0){
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                }else if(n > 0){
                    for(int j = 0; j < n; j++){
                        buf[j] = toupper(buf[j]);
                    }
                    write(sockfd, buf, n);
                    write(STDOUT_FILENO, buf, n);
                }
                if(--nready == 0){
                    break;
                }
            }
        }
    }

    return 0;
}