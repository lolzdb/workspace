#include "../hall.h"

pthread_mutex_t recordlock=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t recordcon=PTHREAD_COND_INITIALIZER;
int readr=0,writer=0;

int *record;
struct nodeInfo *array;
pthread_barrier_t barrier;

char *filename;
char *path;
int scale;
int filesize;
char *recordpath;
int recordfile;

void wRecordLock(){
    pthread_mutex_lock(&recordlock);
    writer=1;
    if(readr>0)pthread_cond_wait(&recordcon,&recordlock);
    pthread_mutex_unlock(&recordlock);
}

void wRecordUnlock(){
    pthread_mutex_lock(&recordlock);
    writer=0;
    pthread_cond_broadcast(&recordcon);
    pthread_mutex_unlock(&recordlock);
}

void rRecordLock(){
    pthread_mutex_lock(&recordlock);
    if(writer>0)pthread_cond_wait(&recordcon,&recordlock);
    readr++;
    pthread_mutex_unlock(&recordlock);
}

void rRecordUnlock(){
    pthread_mutex_lock(&recordlock);
    readr--;
    if(read==0&&writer>0)pthread_cond_signal(&recordcon);
    pthread_mutex_unlock(&recordlock);
}

void getInfo(int sk,struct nodeInfo *array,int scale){
    struct package buf;
    struct nodeInfo *node;
    int i=0;
    while(i<scale){
        recvn(sk,&buf);
        node=(struct nodeInfo *)buf.data;
        array[i].num=node->num;
        array[i].port=node->port;
        strcpy(array[i].ip,node->ip);
        strcpy(array[i].path,node->path);
        i++;
    }
}

void writeRecord(int *record,int count,int fd){
    wRecordLock();
    lseek(fd,0,SEEK_SET);
    write(fd,record,sizeof(int)*count);
    wRecordUnlock();
}

void readRecord(int *record,int count,int fd){
    read(fd,(char *)record,sizeof(int)*count);
}

int linkSever(struct nodeInfo *node,int start,int index){
    int s=con(node->ip,node->port);
    sendn(s,node->path,2,strlen(node->path)+1);
    return s;
}

int download(int sk,int fd,int *record,char *buff,int num,int buffsize){
    int count=0,n,filesize=0;
    struct package buf;
    while(1){
        recvn(sk,&buf);
        if(buf.sum==-1){
            write(fd,buff,count);
            rRecordLock();
            record[num]=filesize;
            rRecordUnlock();
            break;
        }
        count+=buf.sum;
        filesize+=buf.sum;
        if(buffsize-count<1024){
            write(fd,buff,count);
            rRecordLock();
            record[num]=filesize;
            rRecordUnlock();
            count=0;
        }
    }
    return 0;
}

int fusion(char *filename,int sum,char *path){
    int origin,index,n;
    char *num;
    char *file=(char *)malloc(sizeof(char)*512);
    char *filecopy=(char *)malloc(sizeof(char)*512);
    char *databuff=(char *)malloc(sizeof(char)*4096);
    strcpy(file,path);
    strcat(file,"/");
    strcat(file,filename);
    strcpy(filecopy,file);
    strcat(file,"0000");
    origin=open(file,O_RDWR|O_CREAT,0600);
    for(int i=0;i<sum;i++){
        strcpy(file,filecopy);
        num=itoa(i+1);
        printf("num=%s\n",num);
        strcat(file,num);
        index=open(file,O_RDWR);
        while((n=read(index,databuff,4096))>0){
            write(origin,databuff,n);
        }
        free(num);
        unlink(file);
    }
    free(file);
    free(filecopy);
    free(databuff);
    return 1;
}

int comDown(int sk,int *mark,int *record,int scale){
    return 0;
}

int reconstruction(int *mark,int scale,int *record,int filesize){
    int size=filesize/scale;
    int count=0;
    scale--;
    for(int i=0;i<scale;i++){
        if(record[i]<scale){
            mark[i]=1;
            count++;
        }else{
            mark[i]=0;
        }
    }
    if(record[scale]<filesize-size*scale){
        record[scale]=1;
        count++;
    }else record[scale]=0;
    return count;
}

void* getFile(void *args){
    int index=(int)args;
    char *file=(char *)malloc(sizeof(char)*1024);
    char *buff=(char *)malloc(sizeof(char)*20480);
    char *num=itoa(index+1);
    int start=record[index];
    int fd,sk;
    strcpy(file,path);
    strcat(file,"/0");
    strcat(file,filename);
    strcat(file,num);
    fd=open(file,O_CREAT|O_RDWR,0600);
    sk=linkSever(&array[index],record[index],index);
    sendn(sk,(char *)&start,1,sizeof(int));
    download(sk,fd,record,buff,index,20480);
    pthread_barrier_wait(&barrier);
    close(fd);
    free(num);
    free(file);
    free(buff);
}

int reDonwn(int sk){  //qu shao jilu wen jian 
    int *mark=(int *)malloc(sizeof(int)*10);
    int count;
    pthread_t ph;
    readRecord(record,scale,recordfile);
    count=reconstruction(mark,scale,record,filesize);
    getInfo(sk,array,scale);
    pthread_barrier_init(&barrier,NULL,count);
    for(int i=0;i<scale;i++){
        if(mark[i]>0)
            pthread_create(&ph,NULL,getFile,(void *)i);
    }
    pthread_barrier_wait(&barrier);
    pthread_barrier_destroy(&barrier);
    fusion(filename,scale,path);
    free(mark);
    return 1;
}

void recordFile(char *path,char *filename){
    char *file=(char *)malloc(sizeof(char)*1024);
    strcpy(file,path);
    strcat(file,"/");
    strcat(file,filename);
    strcat(file,"-record");
    recordpath=file;
    recordfile=open(file,O_CREAT|O_RDWR,0600);
}

void init(int sk,int scal,char *pa){
    filename=(char *)malloc(sizeof(char)*1024);
    path=(char *)malloc(sizeof(char)*1024);
    array=(struct nodeInfo *)malloc(sizeof(struct nodeInfo)*scal);
    record=(int *)malloc(sizeof(int)*1024);
    scale=scal;
    recvMsg(sk,&filesize,filename);
    recordFile(pa,filename);
    strcpy(path,pa);
}

void clear(){
    close(recordfile);
    unlink(recordpath);
    free(recordpath);
    free(record);
    free(path);
    free(array);
    free(array);
}

int downloads(int sk){
    pthread_t ph;
    char data[32]="end";
    getInfo(sk,array,scale);
    pthread_barrier_init(&barrier,NULL,scale+1);
    for(int i=0;i<scale;i++){
        record[i]=0;
        pthread_create(&ph,NULL,getFile,(void *)i);
    }
    pthread_barrier_wait(&barrier);
    pthread_barrier_destroy(&barrier);
    fusion(filename,scale,path);
    sendn(sk,data,2,32);
    return 1;
}

