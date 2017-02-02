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
#include<sys/sendfile.h>
#include<sys/stat.h>
struct message{
  int type;
  char filename[512];
};

int openfile(struct message ms){
    int file;
    if(ms.type!=1)return 0;
    file=open(ms.filename,O_RDONLY);
    if(file<0){
        perror("open error");
        return 0;
    }
    return file;
}

int create(char *addr,int port){
    struct sockaddr_in sever;
    int s,len;
    if((s=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("socket ");
        exit(1);
        return -1;
    }
    memset(&sever,0,sizeof(sever));
    sever.sin_family=AF_INET;
    sever.sin_port=htons(port);
    sever.sin_addr.s_addr=inet_addr(addr);
    if(bind(s,(struct sockaddr *)&sever,sizeof(sever))<0){
        perror("bind ");
        return -1;
    }
    if(listen(s,8)<0){
        perror("bind ");
        return -1;
    }
    return s;
}

int getsize(struct message ms){
    struct stat buff;
    if(stat(ms.filename,&buff)<0){
        perror("getsize ");
        return 0;
    }
    return buff.st_size;
}


int trans(int fd,int socket,int size){
    int count;
    if((count=sendfile(socket,fd,NULL,size))<0){
        perror("sendfile  ");
        return 0;
    }
    return 1;
}


int main(){
    struct message ms;
    int s=create("127.0.0.1",10701);
    struct sockaddr_in client;
    int len,fd,size;
    int ck=accept(s,(struct sockaddr *)&client,&len);
    recv(ck,&ms,sizeof(ms),0);
    fd=openfile(ms);
    size=getsize(ms);
    trans(fd,ck,size);
    return 0;
}