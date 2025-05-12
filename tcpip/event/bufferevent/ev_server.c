#include <stdio.h>  
#include <unistd.h>  
#include <stdlib.h>  
#include <sys/stat.h>  
#include <string.h>  
#include <fcntl.h>  
#include <event2/event.h>  
#include <event2/listener.h>
#include <event2/bufferevent.h>

#define SERV_PORT 8888

void read_cb(struct bufferevent* bev, void *arg){
    char buf[1024] = {0};
    bufferevent_read(bev, buf, sizeof(buf));
    printf("client say: %s\n", buf);

    char* p = "server say: received\n";
    bufferevent_write(bev, p, strlen(p) + 1);
    
    sleep(1);
}

void write_cb(struct bufferevent* bev, void* arg){
    printf("write successful, cb activated...\n");
}

void event_cb(struct bufferevent* bev, short events, void* arg){
    if(events & BEV_EVENT_EOF){
        printf("connection closed...\n");
    }else if(events & BEV_EVENT_ERROR){
        printf("err...\n");
    }
    bufferevent_free(bev);
    printf("bufferevent freeing\n");
}

void cb_listener(struct evconnlistener * listener, 
    evutil_socket_t fd, 
    struct sockaddr * addr, 
    int socklen, void* ptr){

    struct event_base* base = (struct event_base*)ptr;
    
    struct bufferevent* bev;
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bev, read_cb, write_cb, event_cb, NULL);
    bufferevent_enable(bev, EV_READ);
    return;
}

int main(int argc, char* argv){
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    serv.sin_port = htons(SERV_PORT);

    struct event_base* base;
    base = event_base_new();

    struct evconnlistener* listener;
    listener = evconnlistener_new_bind(base, cb_listener, base,
        LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1, 
        (struct sockaddr*)&serv, sizeof(serv));

    event_base_dispatch(base);

    evconnlistener_free(listener);
    event_base_free(base);

    return 0;
}