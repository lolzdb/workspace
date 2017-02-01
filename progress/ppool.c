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

#define MAXNUM 32
#define MAXTHREAD 8

struct message{
  int type;
  char filename[512];
};

int sk;

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

int getsize(struct message ms){
    struct stat buff;
    if(stat(ms.filename,&buff)<0){
        perror("getsize ");
        return 0;
    }
    return buff.st_size;
}

int getport(int port[],int socket){
    struct message ms;
    for(int i;i<MAXTHREAD;i++){
        recv(socket,&ms,sizeof(ms),0);
        if(ms.type<2)return 0;
        port[i]=ms.type;
    }
    return 1;
}

int create(char *addr,int port){
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
    if(bind(s,(struct sockaddr *)&sever,sizeof(sever))<0){
        perror("bind ");
        return -1;
    }
    if(listen(s,MAXNUM)<0){
        perror("bind ");
        return -1;
    }
    return s;
}

int con(in_addr_t addr,int port){
    struct sockaddr_in sever;
    int s;
    if((s=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("socket ");
        return -1;
    }
    memset(&sever,0,sizeof(sever));
    sever.sin_family=AF_INET;
    sever.sin_port=htons(port);
    sever.sin_addr.s_addr=addr;
    if((connect(s,(struct sockaddr *)&sever,sizeof(sever)))<0){
        perror("connect ");
        return -1;
    }
    return s;
}

void *work(void *ad){
    void **argc=ad;
    int id=*(int *)argc[0];
    char **pfile=(char **)argc[1],*file;
    int *size=(int *)argc[2];
    int *socket=(int *)argc[6];
    int end,start,s;
    int *port=(int *)argc[7];
    int i;
    struct sockaddr_in *c=(struct sockaddr_in *)argc[8];
    pthread_mutex_t *lock=(pthread_mutex_t *)argc[3];
    pthread_cond_t *queue=(pthread_cond_t *)argc[4];
    pthread_barrier_t *cond=(pthread_barrier_t *)argc[5];
    while(1){
        pthread_mutex_lock(lock);
        pthread_cond_wait(queue,lock);
        pthread_mutex_unlock(lock);
        file=*pfile;
        s=con(c->sin_addr.s_addr,*port);
        start=(*size/8)*id;
        end=(*size/8)*(id+1);
        if(id==8)end=*size;
        for(i=start;i<end;i+=1024){
            send(s,&file[i],1024,0);
        }
        if(*size%1024!=0){
            send(s,&file[i-1024],end-(i-1024),0);
        }
        close(s);
    }
}

int lead(){
    struct sockaddr_in sever,client;
    pthread_t tpool[MAXTHREAD];
    pthread_mutex_t lock;
    pthread_cond_t queue;
    pthread_barrier_t cond;
    struct message ms;
    void *argc[9];
    int ck,len,file,size,port[MAXTHREAD];
    char *buff,**pbuff;
    pbuff=&buff;
    len=sizeof(sever);
    pthread_mutex_init(&lock,NULL);
    pthread_cond_init(&queue,NULL);
    pthread_barrier_init(&cond,0,9);
    argc[1]=(void *)&buff;
    argc[2]=(void *)&size;
    argc[3]=(void *)&lock;
    argc[4]=(void *)&queue;
    argc[5]=(void *)&cond;
    argc[6]=(void *)&ck;
    argc[8]=(void *)&client;
    for(int i=0;i<MAXTHREAD;i++){
        argc[0]=(void *)&i;
        argc[7]=(void *)&port[i];
        pthread_create(&tpool[i],NULL,work,&argc);
    }
    while(1){
        ck=accept(sk,(struct sockaddr *)&client,&len);
        recv(ck,&buff,sizeof(ms),0);
        if((file=openfile(ms))==0)continue;
        if((size=getsize(ms))==0)continue;
        ms.type=2;
        sprintf(ms.filename,"%s",size);
        send(ck,&ms,sizeof(ms),0);
        if(getport(port,ck)==0)continue;
        buff=(unsigned char *)mmap(0,4096*4,PROT_READ|PROT_WRITE,MAP_SHARED,file,0);
        for(int i=0;i<MAXNUM;i++){
            pthread_cond_signal(&queue);
        }
        pthread_barrier_wait(&cond);
        munmap(buff,4096*4);
        close(ck);
    }
}

int main(){
    pid_t progress[MAXNUM];
    int i;
    sk=creat("127.0.0.1",10701);
    for(i=0;i<MAXNUM;i++){
         if((progress[i]=fork())<0){
             perror("fork ");
             break;
         }else if(progress[i]==0)break;
    }
    if(i<32){
        lead();
    }else{
        for(int j=0;j<MAXNUM;j++){
            wait(NULL);
        }
    }
    return 0;
}
