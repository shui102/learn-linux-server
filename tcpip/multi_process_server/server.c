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

#define SERV_PORT 9527

void sys_err(const char* str){
    perror(str);
    exit(1);
}

void catch_child(int signum){
    while(waitpid(0, NULL, WNOHANG) > 0);
    return;
}


int main(int argc, char* agrv[]){
    int lfd = 0, cfd = 0;
    pid_t pid;
    int ret;
    char buf[BUFSIZ], client_IP[1024];

    struct sockaddr_in serv_addr, clit_addr;
    socklen_t clit_addr_len;
    clit_addr_len = sizeof(clit_addr);

    //memset(&serv_addr, 0, sizeof(serv_addr));
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    lfd = socket(AF_INET, SOCK_STREAM, 0);
    
    bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    
    listen(lfd, 128);

    while(1){

        cfd = accept(lfd, (struct sockaddr *)&clit_addr, &clit_addr_len);

        pid = fork();

        if(pid < 0){
            sys_err("fork err");
        }else if(pid == 0){
            close(lfd);
            break;
        }else{
            struct sigaction act;
            act.sa_handler = catch_child;
            sigemptyset(&act.sa_mask);
            act.sa_flags = 0;
            ret = sigaction(SIGCHLD, &act, NULL);
            if(ret != 0){
                sys_err("sigaction err");
            }
            close(cfd);
            continue;
        }
    }

    if(pid == 0){
        for(;;){
            ret = read(cfd, buf, sizeof(buf));
            if(ret == 0){
                close(cfd);
                exit(1);
            }

            for(int i = 0; i < ret; i++){
                buf[i] = toupper(buf[i]);
            }
            write(cfd, buf, ret);
            write(STDOUT_FILENO, buf, ret);
        }

    }
    
    return 0;
}