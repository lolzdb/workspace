#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>


struct package{
    int sum;
    int mark;
    char data[1024];
};

int createSocket(char *addr,int port,int sum);
int connect(char *addr,int port);
int sendn(int sk,char *data,int type,int size);
void recvn(int sk,struct package *buf);
int sendSmallFile(int sk,int fd,int start,int end);
int sendBigFile(int sk,int fd,int start,int end);
int openfile(char *path);
void getStart(int num,int size,int *size,int *end,int count);
int recvFile(int sk,int fd,int count);
