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
    memset(&ms,0,sizeof(ms));
    for(int i=0;i<MAXTHREAD;i++){
        recv(socket,&ms,sizeof(ms),0);
        if(ms.type<2)return 0;
        port[i]=ms.type;
    }
    return 1;
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
    int index=0,*id=(int *)argc[0];
    pthread_mutex_t *lock=(pthread_mutex_t *)argc[1];
    pthread_cond_t *queue=(pthread_cond_t *)argc[2];
    pthread_barrier_t *cond=(pthread_barrier_t *)argc[3];
    char *file,**pfile=(char **)argc[4];
    int *size=(int *)argc[5];
    int *port=(int *)argc[6];
    int socket,start,end,i;
    struct sockaddr_in *client=(struct sockaddr_in *)argc[7];
    pthread_mutex_lock(lock);
    index=(*id)++;
    pthread_mutex_unlock(lock);
    while(1){
        pthread_mutex_lock(lock);
        pthread_cond_wait(queue,lock);
        pthread_mutex_unlock(lock);
        socket=con(client->sin_addr.s_addr,port[index]);
        printf("my id is %d  port is %d\n",index,port[index]);
        file=*pfile;
        start=(*size/8)*index;
        end=(*size/8)*(index+1);
        if(index==8)end=*size;
        for(i=start;i<end;i+=1024){
            send(socket,&file[i],1024,0);
        }
        if(*size%1024!=0){
            send(socket,&file[i-1024],end-(i-1024),0);
        }
        printf("%d finish work\n",index);
        close(socket);
    }
}

int lead(){
    struct sockaddr_in sever,client;
    pthread_t tpool[MAXTHREAD];
    pthread_mutex_t lock;
    pthread_cond_t queue;
    pthread_barrier_t cond;
    struct message ms;
    void *argc[8];
    int ck,len,file,size,port[MAXTHREAD],id=0;
    char *buff;
    len=sizeof(sever);
    argc[0]=&id;
    argc[1]=&lock;
    argc[2]=&queue;
    argc[3]=&cond;
    argc[4]=&buff;
    argc[5]=&size;
    argc[6]=port;
    argc[7]=&client;
    pthread_mutex_init(&lock,NULL);
    pthread_cond_init(&queue,NULL);
    pthread_barrier_init(&cond,0,9);
    for(int i=0;i<MAXTHREAD;i++){
        pthread_create(&tpool[i],NULL,work,(void *)argc);
    }
    while(1){
        ck=accept(sk,(struct sockaddr *)&client,&len);
        recv(ck,&ms,sizeof(ms),0);
        perror("recv ");
        printf("filename=%s  type=%d\n",ms.filename,ms.type);
        if((file=openfile(ms))==0)continue;
        if((size=getsize(ms))==0)continue;
        ms.type=2;
        printf("size=%d\n",size);
        sprintf(ms.filename,"%d",size);
        send(ck,&ms,sizeof(ms),0);
        if(getport(port,ck)==0)continue;
        buff=mmap(0,size,PROT_READ,MAP_SHARED,file,0);
        for(int i=0;i<MAXNUM;i++){
            pthread_cond_signal(&queue);
        }
        pthread_barrier_wait(&cond);
        munmap(buff,size);
        close(ck);
    }
}

int main(){
    pid_t progress[MAXNUM];
    struct sockaddr_in sever,client;
    int i,ck,len;
    sk=create("127.0.0.1",10701);
    for(i=0;i<1;i++){
         if((progress[i]=fork())<0){
             perror("fork ");
             break;
         }else if(progress[i]==0)break;
    }
    if(i<1){
        lead();
    }else{
        for(int j=0;j<MAXNUM;j++){
            wait(NULL);
        }
    }
    return 0;
}
