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

#define MAXTHREAD 8

struct message{
  int type;
  char filename[512];
};

int por[MAXTHREAD];
int sk[MAXTHREAD];
char ip[]="127.0.0.1";
int size;
pthread_barrier_t queue;

int create(char *addr){
    struct sockaddr_in sever;
    int s,port;
    if((s=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("socket ");
        return -1;
    }
    memset(&sever,0,sizeof(sever));
    sever.sin_family=AF_INET;
    sever.sin_addr.s_addr=inet_addr(addr);
    for(int i=0;i<MAXTHREAD;i++){
        sever.sin_port=htons(port);
        if((s=socket(AF_INET,SOCK_STREAM,0))<0){
            perror("socket ");
            return -1;
        }
        if(bind(s,(struct sockaddr *)&sever,sizeof(sever))<0){
            perror("bind ");
            close(s);
            port++;
            continue;
        }
        if(listen(s,2)<0){
            perror("listen ");
            return -1;
        }
        sk[i]=s;
        por[i]=port;
    }
    return 1;
}

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

void *work(void *argc){
    void **x=argc;
    int id=*(int *)x[1];
    char *buff=*(char **)x[0];
    struct sockaddr_in client;
    int len,count,i;
    int socket=accept(sk[id],(struct sockaddr *)&client,&len);
    printf(" id=%d ",id)
    perror("accept ");
    return NULL;
    i=(size/MAXTHREAD)*i;
    while(i<size){
        count=recv(socket,&buff[i],1024,0);
        i+=1024;
    }
    if(i<size){
        count=recv(socket,&buff[i],size-(i-1024),0);
    }
    close(socket);
    pthread_barrier_wait(&queue);
}

int main(){
    int s=con(ip,10701);
    pthread_t pool[MAXTHREAD];
    struct message ms={1,"hello"};
    int file;
    char *buff;
    void *argc[3];
    pthread_barrier_init(&queue,0,9);
    argc[0]=&buff;
    send(s,&ms,sizeof(ms),0);
    recv(s,&ms,szieof(ms),0);
    sprintf(size,"%d",ms.filename);
    file=open("hello_copy",O_CREAT|O_RDWR);
    ftruncate(file,size);
    buff=(unsigned char *)mmap(0,size,PROT_READ|PROT_WRITE,MAP_SHARED,file,0);
    if(create("127.0.0.1")<0)return 0;
    for(int i=0;i<MAXTHREAD;i++){
        argc[1]=i;
        pthread_create(pool[i],NULL,work,&argc);
    }
    pthread_barrier_wait(&queue);
    munmap(buff,size);
    close(s);
    return 0;
}