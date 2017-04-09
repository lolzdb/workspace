#include<stdio.h>
#include<string.h>
#include<stdlib.h>
//#include "head/backups.h"
//#include "head/transfer.h"
#include "hall.h"

int main(){
    char *file=(char *)malloc(sizeof(char)*1024);
    char *filename=(char *)malloc(sizeof(char)*1024);
    char path[1024]="/home/xws/workspace/linux/ftp";
    int sk;
    strcpy(filename,"a.txt");
    sk=con("127.0.0.1",10702);
    init(sk,2,path);
    downloads(sk);
    return 0;
}

/*
    

    int sk=con("127.0.0.1",10701);
    struct package buf;
    while(1){
        recvn(sk,&buf);
    }
    return 0;
*/
