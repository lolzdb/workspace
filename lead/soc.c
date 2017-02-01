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

void send_file(int file,int sk){
    struct msghdr ms;
    char buff[CMSG_SPACE(sizeof(int))];
    struct iovec iov;
    struct cmsghdr *cms;
    char sendchar='\0';
    int *fd;
    ms.msg_name=NULL;
    ms.msg_namelen=0;
    ms.msg_flags=0;
    iov.iov_base=&sendchar;
    iov.iov_len=sizeof(char);
    ms.msg_iov=&iov;
    ms.msg_iovlen=1;
    cms=(struct cmsghdr *)buff;
    cms->cmsg_len=CMSG_LEN(sizeof(int));
    cms->cmsg_level=SOL_SOCKET;
    cms->cmsg_type=SCM_RIGHTS;
    fd=(int *)CMSG_DATA(cms);
    *fd=file;
    ms.msg_control=cms;
    ms.msg_controllen=CMSG_SPACE(sizeof(int));
    if(sendmsg(sk,&ms,0)<0){
        perror("send ");
        exit(1);
    }
}

void recvfile(int sk){
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
    printf("space=%d  len=%d\n",CMSG_SPACE(sizeof(int)),CMSG_LEN(sizeof(int)));
    if(recvmsg(sk,&ms,0)<0){
        perror("recv ");
        exit(1);
    }
    fd=(int *)CMSG_DATA(cms);
    printf("the file is %d\n",*fd);
    write(*fd,con,strlen(con));
}

/*int main(){
    int sk[2],file;
    pid_t pid;
    if(socketpair(AF_UNIX,SOCK_STREAM,0,sk)<0){
        perror("create ");
        exit(1);
    }
    pid=fork();
    if(pid<0){
        perror("fork");
        exit(2);
    }
    else if(pid==0){
        close(sk[1]);
        recvfile(sk[0]);
    }
    else{
        close(sk[0]);
        if((file=open("a.txt",O_CREAT|O_RDONLY|O_WRONLY),0666)<0){
            perror("file ");
            exit(3);
        }
        
        send_file(file,sk[1]);
    }
    return 0;
}*/
int main(){
    int file,sk,len,ck;
    struct sockaddr_un sever;
    char pathname[]="socketfile";
    len=offsetof(struct sockaddr_un,sun_path)+strlen(pathname);
    printf("len=%d\n",len);
    sk=socket(AF_UNIX,SOCK_STREAM,0);
    perror("create ");
    memset(&sever,0,sizeof(sever));
    sever.sun_family=AF_UNIX;
    strcpy(sever.sun_path,pathname);
    unlink(pathname);
    bind(sk,(struct sockaddr *)&sever,len);
    perror("bind ");
    listen(sk,12);
    perror("listen ");
    ck=accept(sk,(struct sockaddr *)&sever,&len);
    perror("accept");
    if((file=open("a.txt",O_CREAT|O_RDONLY|O_WRONLY),0666)<0){
            perror("file ");
            exit(3);
        }
        
    send_file(file,ck);
    return 0;
}
//proc/sys/net/core/opt‐mem_max