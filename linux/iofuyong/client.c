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
#include<sys/select.h>
#include<sys/stat.h>


int con(char *addr,int port){
    struct sockaddr_in sever;
    int s;
    if((s=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("socket ");
        return -1;
    }
    memset(&sever,0,sizeof(sever));
    sever.sin_family=AF_INET;
    sever.sin_port=htons(port);
    sever.sin_addr.s_addr=inet_addr(addr);
    if((connect(s,(struct sockaddr *)&sever,sizeof(sever)))<0){
        perror("connect ");
        return -1;
    }
    return s;
}

int main(){
    int s=con("127.0.0.1",10701);
    fd_set read;
    int len,max=s+1;
    char buff[512];
    struct timeval time;
    time.tv_sec=1;
    time.tv_usec=0;
    FD_SET(s,&read);
    FD_SET(STDIN_FILENO,&read);
    while(1){
        FD_ZERO(&read);
        FD_SET(s,&read);
        FD_SET(STDIN_FILENO,&read);
        len=select(max,&read,NULL,NULL,&time);
        if(len<=0)continue;
        if(FD_ISSET(s,&read)){
            len=recv(s,buff,512,0);
            buff[len]=0;
            printf("recv %s\n",buff);
            FD_CLR(s,&read);
            FD_SET(s,&read);
        }
        if(FD_ISSET(STDIN_FILENO,&read)){
            fgets(buff,512,stdin);
            send(s,buff,strlen(buff),0);
            FD_CLR(s,&read);
            FD_SET(s,&read);
        }
    }
   return 0;
}