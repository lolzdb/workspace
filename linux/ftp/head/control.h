#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>



int sendMsg(int sk,int size,char *filename);
void recvMsg(int sk,int *size,char *filename);