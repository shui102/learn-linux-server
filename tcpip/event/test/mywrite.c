#include <stdio.h>  
#include <unistd.h>  
#include <stdlib.h>  
#include <sys/stat.h>  
#include <string.h>  
#include <fcntl.h>  
#include <event2/event.h>  


void write_cb(evutil_socket_t fd, short what, void* arg){

    char buf[] = "hello libevent \n";

    int len = write(fd, buf, strlen(buf) + 1);
    
    sleep(1);
    return;
}

int main(int argc, char* argv){
    

    int fd = open("testfifo", O_WRONLY | O_NONBLOCK);
    if(fd == -1){
        perror("open err");
    }

    struct event_base* base = event_base_new();
    //创建事件
    struct event *ev = event_new(base, fd, EV_WRITE | EV_PERSIST, write_cb, NULL);
    //添加事件到event_base上
    event_add(ev, NULL);
    //启动循环
    event_base_dispatch(base);
    event_base_free(base);

    return 0;
}