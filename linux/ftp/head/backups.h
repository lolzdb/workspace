#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>


void sendpackage(int fd,char *filename,char *ip,int port,int num,int end,char *path,int start);
void savepackage(int sk);
