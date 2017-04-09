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

int main(){
    int sk=create("127.0.0.1",10701);
    int ck,len,count,user[500],id=0,max;
    fd_set read;
    struct timeval time;
    struct sockaddr_in client;
    char buff[512];
    time.tv_sec=1;
    time.tv_usec=0;
    perror("create");
    FD_SET(sk,&read);
    max=sk+3;
    while(1){
        FD_ZERO(&read);
        FD_SET(sk,&read);
        for(int i=0;i<=id;i++){ 
            FD_SET(user[i],&read);
        }
        len=select(max,&read,NULL,NULL,&time);
        if(len<=0)continue;
        if(FD_ISSET(sk,&read)){
            ck=accept(sk,(struct sockaddr *)&client,&len);
            user[id++]=ck;
            max=ck+1;
        }
        for(int i=0;i<=id;i++){ 
            if(FD_ISSET(user[i],&read)){
                len=recv(user[i],buff,512,0);
                buff[len]=0;
                printf("%s",buff);
                for(int j=0;j<=id;j++){
                    if(j!=i)send(user[j],buff,len,0);
                }
            }
        }
    }
    return 0;
}