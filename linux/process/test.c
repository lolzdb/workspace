#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<errno.h>
#include<unistd.h>
#include<sys/un.h>
#include<fcntl.h>
#include<sys/types.h>
#include<stddef.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<sys/epoll.h>


int spipe[8][2];

int main(){
    pid_t pid;
    int get;
    socketpair(AF_UNIX,SOCK_STREAM,0,spipe[0]);
    if((pid=fork())<0)return 0;
    if(pid==0){
        close(spipe[0][0]);
        recv(spipe[0][1],&get,sizeof(get),0);
        printf("chile pipe is %d\n",spipe[0][1]);
    }else{
        close(spipe[0][1]);
        send(spipe[0][1],&get,sizeof(get),0);
        printf("parent pipe is %d\n",spipe[0][0]);
    }
    return 0;
}