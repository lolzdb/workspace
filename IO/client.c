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

struct mssage{
    int type;
    char buff[512];
};

int sk;

void *recvfile(void *argc){
    int file=open("hello_copy",O_CREAT|O_WRONLY,0666),count;
    perror("open ");
    char buff[1024];
    if(file<0){
        perror("file ");
         return 0;
    }
    while(1){
        count=recv(sk,buff,1024,0);
        perror("recv ");
        if(count==0)break;
        count=write(file,buff,count);
        perror("write ");
        if(count<0){
            unlink("hello-copy");
            break;
        }
    }
    close(sk);
}

int main(){
    struct sockaddr_in sever;
    int len=sizeof(sever);
    char filename[]="hello";
    struct mssage ms;
    pthread_t re;
    sk=socket(AF_INET,SOCK_STREAM,0);
    memset(&sever,0,len);
    sever.sin_family=AF_INET;
    sever.sin_port=htons(10701);
    sever.sin_addr.s_addr=inet_addr("127.0.0.1");
    if(connect(sk,(struct sockaddr *)&sever,len)<0){
         perror("connect ");
         return 0;
    }
    pthread_create(&re,NULL,recvfile,NULL);
    ms.type=1;
    strcpy(ms.buff,filename);
    send(sk,&ms,sizeof(ms),0);
    pthread_join(re,NULL);
    return 1;
}