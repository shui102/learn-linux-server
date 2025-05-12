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
#define MAX_EVENTS 1024
#define BUFLEN 4096

//回调函数
void recvdata(int fd, int events, void *arg);
void senddata(int fd, int events, void *arg);

struct myevent_s{
    int fd;         //当前socket操作符
    int events;     //当前状态，是输入还是读出
    void *arg;      //当前结构体在数组的位置
    void (*call_back)(int fd, int events, void* arg);   //回调函数
    int status;     //若为0则不在监听，1则在监听
    char buf[BUFLEN];   //缓冲区    
    int len;            //缓冲区的大小  
    long last_active;   //上一次活动的时间
};

int g_efd;  //监听树的根节点
struct myevent_s g_events[MAX_EVENTS + 1];  //已经存在的连接

void eventset(struct myevent_s *ev, int fd, void (*call_back)(int, int, void *), void *arg){
    ev->fd = fd;
    ev->call_back = call_back;
    ev->events = 0;
    ev->arg = arg;
    ev->status = 0;
    if(ev->len <= 0){
        memset(ev->buf, 0, sizeof(ev->buf));
        ev->len = 0;
    }
    ev->last_active = time(NULL);
    return;
}

//添加到监听红黑树，设置监听事件
void eventadd(int efd, int events, struct myevent_s *ev){
    struct epoll_event epv = {0, {0}};
    int op = 0;
    epv.data.ptr = ev;
    epv.events = ev->events = events;

    if(ev->status == 0){    //当该事件没有被监听时
        op = EPOLL_CTL_ADD;
        ev->status = 1;
    }

    if(epoll_ctl(efd, op, ev->fd, &epv) < 0){
        printf("event add failed [fd = %d], events[%d]\n", ev->fd, events);
    } else {
      printf("event add OK [fd = %d], op = %d, events[%0x]\n", ev->fd, op, events);
    }
    return ;  
}

void eventdel(int efd, struct myevent_s *ev){
    struct epoll_event epv = {0, {0}};

    if(ev->status != 1){
        return;
    }
    epv.data.ptr = NULL;
    ev->status = 0;
    epoll_ctl(efd, EPOLL_CTL_DEL, ev->fd, &epv);
    return;
}

void acceptconn(int lfd, int events, void *arg){
    struct sockaddr_in cin;
    socklen_t len = sizeof(cin);
    int cfd, i;

    if((cfd = accept(lfd, (struct sockaddr *)&cin, &len)) == -1){
        if(errno != EAGAIN && errno != EINTR){

        }
        printf("%s:accept, %s\n", __func__ , strerror(errno));
        return ;
    }

    do{
        for(i = 0; i < MAX_EVENTS; i++){
            if(g_events[i].status == 0){
                break;
            }
        }
        if(i == MAX_EVENTS){
            printf("%s: max connect limit[%d]\n", __func__, MAX_EVENTS);
            break;
        }
        int flag = 0;
        if((flag = fcntl(cfd, F_SETFL, O_NONBLOCK)) < 0){
            printf("%s:fcntl no-blocking error, %s\n", __func__, strerror(errno));
            break;
        }

        eventset(&g_events[i], cfd, recvdata, &g_events[i]);
        eventadd(g_efd, EPOLLIN, &g_events[i]);
    }while(0);

    printf("new connect [%s:%d][time:%ld], pos[%d]\n",
        inet_ntoa(cin.sin_addr), ntohs(cin.sin_port), g_events[i].last_active, i);
 return ;
}

void recvdata(int fd, int events, void *arg){
    struct myevent_s *ev = (struct myevent_s *)arg;
    int len;

    len = recv(fd, ev->buf, sizeof(ev->buf), 0);
    eventdel(g_efd, ev);

    if(len > 0){
        ev->len = len;
        ev->buf[len] = '\0';
        printf("C[%d]:%s\n", fd, ev->buf);

        eventset(ev, fd, senddata, ev);
        eventadd(g_efd, EPOLLOUT, ev);
    }else if(len == 0){
        close(ev->fd);
        printf("[fd = %d] pos[%ld], closed\n", fd, ev - g_events);
    } else {                // 若存在异常
      close(ev->fd);
      printf("revc[fd = %d] error[%d]:%s\n", fd, errno, strerror(errno));
    }
    
}

void senddata(int fd, int events, void * arg){
    struct myevent_s *ev = (struct myevent_s *)arg;
    int len;
    for(int i = 0; i < ev->len; i++){
        ev->buf[i] = toupper(ev->buf[i]);
    }
    len = write(fd, ev->buf, ev->len);
    eventdel(g_efd, ev);
    if (len > 0) {
        printf("send[fd = %d], [%d]%s\n", fd, len, ev->buf);
        eventset(ev, fd, recvdata,ev);    // 由监听写改为监听读
        eventadd(g_efd, EPOLLIN, ev);
      } else {
        close(ev->fd);
        printf("send[fd = %d]error len = %d ,%s\n", fd, len, strerror(errno));
      }
      return ;
}

void initlistensocket(int efd, short port){
    struct sockaddr_in sin;
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(lfd, F_SETFL, O_NONBLOCK);

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);

    bind(lfd, (struct sockaddr *)&sin, sizeof(sin));

    listen(lfd, 20);
    //将lfd放在数组最后
    eventset(&g_events[MAX_EVENTS], lfd, acceptconn, &g_events[MAX_EVENTS]);

    eventadd(efd, EPOLLIN, &g_events[MAX_EVENTS]);

    return;
}


void sys_err(const char* str){
    perror(str);
    exit(1);
}

int main(int argc, char* argv[]){
    unsigned short port = SERV_PORT;
    if(argc == 2){
        port = atoi(argv[1]);
    }

    g_efd = epoll_create(MAX_EVENTS + 1);
    if(g_efd < 0){
        printf("create efd in %s err %s\n", __func__ , strerror(errno));
    }

    initlistensocket(g_efd, port);
    struct epoll_event events[MAX_EVENTS + 1];
    printf("server running :port[%d]\n", port);

    int checkpos = 0, i;
    while(1){
        long now = time(NULL);

        for(i = 0; i < 100; i++, checkpos ++){
            if(checkpos == MAX_EVENTS){
                checkpos = 0 ;
            }
            if(g_events[checkpos].status != 1){
                continue;
            }

            long duration = now - g_events[checkpos].last_active;
            if(duration >= 60){
                close(g_events[checkpos].fd);
                printf("[fd = %d] timeout\n", g_events[checkpos].fd);
                eventdel(g_efd, &g_events[checkpos]);
            }
        }

        int nfd = epoll_wait(g_efd, events, MAX_EVENTS + 1, 1000);
        if(nfd < 0){
            printf("epoll wait err\n");
            exit(-1);
        }

        for(i = 0; i < nfd; i++){
            struct myevent_s *ev = (struct myevent_s *)events[i].data.ptr;

            if ((events[i].events & EPOLLIN) && (ev->events & EPOLLIN)) {
                ev->call_back(ev->fd, events[i].events, ev->arg);
              }
        
              if ((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT)) {
                ev->call_back(ev->fd, events[i].events, ev->arg);
              }
        }
    }
    return 0;
}