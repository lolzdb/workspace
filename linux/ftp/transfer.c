//#include "head/transfer.h"
#include "hall.h"
int createSocket(char *addr,int port,int sum){
  int sk;
  struct sockaddr_in ip;
  sk=socket(AF_INET,SOCK_STREAM,0);
  if(sk==-1){
      perror("socket ");
      return 0;
  }
  memset(&ip,0,sizeof(ip));
  ip.sin_family=AF_INET;
  ip.sin_port=htons(port);
  ip.sin_addr.s_addr=inet_addr(addr);
  if(bind(sk,(struct sockaddr *)&ip,sizeof(ip))<0){
       perror("bind ");
       return 0;
  }
  if(listen(sk,sum)<0){
      perror("listen ");
       return 0;
  }
  return sk;
}

int con(char *addr,int port){
  int sk;
  struct sockaddr_in ip;
  sk=socket(AF_INET,SOCK_STREAM,0);
  if(sk==-1){
      perror("socket ");
      return 0;
  }
  memset(&ip,0,sizeof(ip));
  ip.sin_family=AF_INET;
  ip.sin_port=htons(port);
  ip.sin_addr.s_addr=inet_addr(addr);
  if(connect(sk,(struct sockaddr *)&ip,sizeof(ip))<0){
       perror("connect ");
       return 0;
  }
  return sk;
}

int sendn(int sk,char *data,int type,int size){
  struct package buf;
  int n=0;
  buf.sum=size;
  buf.mark=type;
  memcpy(buf.data,data,size);
  n=send(sk,&buf,sizeof(int)*2+size,0);
  return 0;
}

void recvn(int sk,struct package *buf){
   int sum,size=0,count;
   recv(sk,&sum,sizeof(int),MSG_WAITALL);
   recv(sk,&buf->mark,sizeof(int),MSG_WAITALL);
   buf->sum=sum;
   while(sum>size){
      count=recv(sk,&buf->data[size],sum-size,0);
      size+=count;
   }
   buf->data[sum]=0;
}

int sendSmallFile(int sk,int fd,int start,int end){
    int count=start+1024,n,filesize=0;
    char data[1025];
    data[1024]=0;
    while(count<end){
        n=read(fd,data,1024);
        if(n<0){
            perror("file perror");
            return -1;
        }else if(n==0){
                printf("end trans\n");
                return 0;
            }
        sendn(sk,data,0,n);
        count+=n;
    }
    n=read(fd,data,end+1024-count);
    data[n]=0;
    sendn(sk,data,0,n);
    close(fd);
    return 0;
}

int sendBigFile(int sk,int fd,int start,int end){
    char *file=mmap(0,start-end,PROT_READ,MAP_SHARED,fd,start);
    int count=1024,size=end-start;
    while(count<size){
        sendn(sk,&file[count],0,1024);
    }
    sendn(sk,&file[count],0,count-size);
    munmap(file,size);
    close(fd);
    return 0;
}

int openFile(char *path){
    int fd=open(path,O_RDWR);
    if(fd<0){
        perror("open file error");
        return -1;
    }
    return fd;
}

void getStart(int num,int size,int *start,int *end,int count){
    int m=size/count;
    *start=(num-1)*m;
    if(num==count)*end=size;
    else *end=(num*m);
}

int recvFile(int sk,int fd,int count){
    struct package* s=(struct package*)malloc(sizeof(struct package));
    int sum=0,n;
    while(sum<count){
        recvn(sk,s);
        sum+=s->sum;
        write(fd,s->data,s->sum);
    }
    return 0;
}

