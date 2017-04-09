//#include "head/control.h"
#include "hall.h"

int sendMsg(int sk,int size,char *filename){
    struct conmsg msg;
    msg.sum=strlen(filename);
    msg.size=size;
    strcpy(msg.data,filename);
    send(sk,&msg,sizeof(int)*2+msg.sum,0);
    return 0;
}

void recvMsg(int sk,int *size,char *filename){
    int sum;
    recv(sk,&sum,sizeof(sum),MSG_WAITALL);
    recv(sk,size,sizeof(int),MSG_WAITALL);
    recv(sk,filename,sum,MSG_WAITALL);
}