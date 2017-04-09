#include "hall.h"



int putInfo(MYSQL *data,char *path,int scale,int sk){
    struct nodeInfo *node=(struct nodeInfo *)malloc(sizeof(struct nodeInfo)*scale);
    char *sql=(char *)malloc(sizeof(char)*1024);
    int count,i=0;
    MYSQL_RES *result;
    MYSQL_ROW row;
    sql[0]=0;
    strcat(sql,"select ip,port,store from file,server where file.server_id=server.id and path='");
    strcat(sql,path);
    strcat(sql,"' order by num");
    result=query(data,sql);
    count=mysql_num_fields(result);
    while((row=mysql_fetch_row(result))!=NULL){
            bzero(&node[i],sizeof(struct nodeInfo));
            strcpy(node[i].ip,row[0]);
            node[i].port=atoi(row[1]);
            strcpy(node[i].path,row[2]);
            printf("ip=%s   port=%d    path=%s\n",node[i].ip,node[i].port,node[i].path);
            i++;
    }
    for(i=0;i<scale;i++){
        sendn(sk,(char *)&node[i],2,sizeof(struct nodeInfo));
    }
    return 1;
}

int putfile(MYSQL *data,int sk,char *path,char *filename,int scale){
    int size=getSize(path,filename);
    char *filepath=(char *)malloc(sizeof(char)*1024);
    struct package buf;
    strcpy(filepath,path);
    strcat(filepath,"/");
    strcat(filepath,filename);
    printf("filename=%s\n",filepath);
    sendMsg(sk,size,filename);
    putInfo(data,filepath,scale,sk);
    recvn(sk,&buf);
    if(strcmp(buf.data,"end")==0){
        return 1;
    }
    return 0;
}

int handleFile(int sk){
    char file[1025];
    struct package buf;
    int fd,start,n;
    recvn(sk,&buf);
    strcpy(file,buf.data);
    printf("filepath=%s",file);
    fd=openFile(file);
    recvn(sk,&buf);
    start=*(int *)buf.data;
    lseek(fd,start,SEEK_SET);
    while((n=read(fd,file,1024))!=0){
        file[n]=0;
        printf("%s",file);
        sendn(sk,file,0,n);
    }
    printf("\n");
    n=-1;
    send(sk,&n,sizeof(int),0);
    return 1;
}

