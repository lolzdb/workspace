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
#include<signal.h>

#define MAXLINK 16
#define MAXNUM 8

int spipe[8][2];

void sighandler(int num){
    printf("recv signal %d\n",num);
}

int socket_create(char *ip,int port){
    struct sockaddr_in sever;
    int sk,len;
    sk=socket(AF_INET,SOCK_STREAM,0);
    sever.sin_family=AF_INET;
    sever.sin_port=htons(port);
    sever.sin_addr.s_addr=inet_addr(ip);
    if(bind(sk,(struct sockaddr *)&sever,sizeof(sever))<0){
        perror("bind error");
        return 0;
    }
    if(listen(sk,MAXLINK)<0){
        perror("bind error");
        return 0;
    }
    return sk;
}

int sendfd(int socket,int fd){
    struct msghdr ms;
    char buff[CMSG_SPACE(sizeof(int))];
    struct cmsghdr *cms=(struct cmsghdr *)buff;
    struct iovec iov;
    char mark='a';
    int *data;
    iov.iov_base=&mark;
    iov.iov_len=sizeof(mark);
    ms.msg_iov=&iov;
    ms.msg_iovlen=1;
    ms.msg_name=NULL;
    ms.msg_namelen=0;
    ms.msg_control=cms;
    ms.msg_controllen=CMSG_SPACE(sizeof(int));
    ms.msg_flags=0;
    cms->cmsg_level=SOL_SOCKET;
    cms->cmsg_type=SCM_RIGHTS;
    cms->cmsg_len=CMSG_LEN(sizeof(int));
    data=(int *)CMSG_DATA(cms);
    *data=fd;
    if(sendmsg(socket,&ms,0)<0){
        perror("sendmsg ");
        return 0;
    }
    return 1;
}

int setNobloc(int fd){
    int val=fcntl(fd,F_GETFL,0);
    val=val|O_NONBLOCK;
    fcntl(fd,F_SETFL,val);
    return 0;
}

int recvfile(int sk){
    struct msghdr ms;
    struct iovec iov;
    char recvchar;
    char con[]="hello world";
    char buff[CMSG_SPACE(sizeof(int))];
    struct cmsghdr *cms=(struct cmsghdr *)buff;
    int *fd;
    ms.msg_name=NULL;
    ms.msg_namelen=0;
    ms.msg_flags=0;
    iov.iov_base=&recvchar;
    iov.iov_len=sizeof(char);
    ms.msg_iov=&iov;
    ms.msg_iovlen=1;
    memset(cms,0,CMSG_SPACE(sizeof(int)));
    ms.msg_control=cms;
    ms.msg_controllen=CMSG_SPACE(sizeof(int));
    if(recvmsg(sk,&ms,0)<0){
        perror("recvfile ");
        exit(1);
    }
    fd=(int *)CMSG_DATA(cms);
    return *fd;
}

int addfd(int ep,int fd){
    struct epoll_event event;
    int ret;
    event.events=EPOLLIN|EPOLLET;
    event.data.fd=fd;
    ret=epoll_ctl(ep,EPOLL_CTL_ADD,fd,&event);
    if(ret<0){
         perror("epoll add error");
         return 0;
    }
    return 1;
}

int chile(int id){
    int ep=epoll_create(1);
    struct sockaddr_in client;
    int clen=0,ret,count,i,fd,ck;
    char buff[512],t[1024];
    struct epoll_event events[32];
    addfd(ep,spipe[id][1]);
    epoll_wait(ep,events,32,-1);
    epoll_ctl(ep,EPOLL_CTL_DEL,spipe[id][1],NULL);
    addfd(ep,spipe[id][1]);
    sprintf(t,"%d severce for you",id);
    while(1){
        ret=epoll_wait(ep,events,32,500);
        if(ret==0)continue;
        for(i=0;i<ret;i++){
            fd=events[i].data.fd;
            if(fd==spipe[id][1]&&(events[i].events&EPOLLIN)){
                ck=recvfile(spipe[id][1]);
                printf("chile  recv  %d\n",ck);
                addfd(ep,ck);
            }else if(events[i].events&EPOLLERR){
                close(fd);
               // perror("the error is ");
               // printf("the %d have error\n",fd);
            }else if(events[i].events&EPOLLIN){
                count=recv(fd,buff,512,0);
                buff[count]=0;
                if(count==0){
                    close(fd);
                    epoll_ctl(ep,EPOLL_CTL_DEL,fd,NULL);
                    continue;
                }
                buff[count-1]=0;
                strcat(t,buff);
                printf("%s\n",t);
                send(fd,t,strlen(t),0);
            }
        }
    }
    return 0;
}

int parent(){
    int sk=socket_create("127.0.0.1",10701);
    int ep=epoll_create(1024);
    int ck,len,count,ret,fd;
    struct sockaddr_in client;
    struct epoll_event events[32];
    struct sigaction act;
    act.sa_handler=sighandler;
    act.sa_flags=SA_RESTART;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT,&act,NULL);
    addfd(ep,sk);
    while(1){
        ret=epoll_wait(ep,events,32,2000);
        if(ret==0)continue;
        for(int i=0;i<ret;i++){
            fd=events[i].data.fd;
            if(fd==sk&&events[i].events&EPOLLIN){
                ck=accept(sk,(struct sockaddr *)&client,&len);
                sendfd(spipe[0][0],ck);
            }
        }
    }
}

int main(){
    pid_t pid;
    int i;
    int get;
    for(i=0;i<1;i++){
        socketpair(AF_UNIX,SOCK_STREAM,0,spipe[i]);
        pid=fork();
        if(pid==0){
            close(spipe[i][0]);
            setNobloc(spipe[i][1]);
            break;
        }
        close(spipe[i][1]);
    }
    if(pid==0){
        chile(i);
    }else{
        parent();
        wait(NULL);
    }
    return 0;
}





