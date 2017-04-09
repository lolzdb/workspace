//#include "head/backups.h"
//#include "head/control.h"
//#include "head/dirent.h"
//#include "head/transfer.h"
#include "hall.h"

char* itoa(int x){
    char *result=(char *)malloc(sizeof(char)*16);
    int i=0;
    char t;
    for(;x>0;i++){
        result[i]=x-(x/10)*10+'0';
        x/=10;
    }
    result[i--]='\0';
    for(int j=0;j<i;j++,i--){
        t=result[j];
        result[j]=result[i];
        result[i]=t;
    }
    return result;
}

int sendpackage(int fd,char *filename,char *ip,int port,int num,int end,char *path,int start){
    int sk=con(ip,port);
    int sum,count=0,n;
    char *se=itoa(num);
    struct package buf;
    if(sk<0){
        perror("connect error ");
        return 0;
    }
    strcat(filename,"-");
    strcat(filename,se);
    free(se);
    sendMsg(sk,end-start,filename);
    sendSmallFile(sk,fd,start,end);
    recvn(sk,&buf);
    printf("path=%s\n",buf.data);
    strcpy(path,buf.data);
}

void savepackage(int sk){
    int size,fd;
    char *filename=(char *)malloc(sizeof(char)*1024);
    char path[512];
    recvMsg(sk,&size,filename);
    getcwd(path,512);
    createFile(path,filename,size);
    strcat(path,"/");
    strcat(path,filename);
    fd=openFile(path);
    recvFile(sk,fd,size);
    sendn(sk,path,0,strlen(path));
}