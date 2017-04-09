#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<pthread.h>
#include<mysql/mysql.h>

struct conmsg{
    int sum;
    int size;
    char data[256];
};

struct package{
    int sum;
    int mark;
    char data[1025];
};

struct stack{
    DIR *data;
    int start;
};

struct nodeInfo{
    int num;
    int port;
    char ip[16];
    char path[512];
};

struct pk{
    int a;
    int b;
    int c;
    char d[128];
};


int sendpackage(int fd,char *filename,char *ip,int port,int num,int end,char *path,int start);
void savepackage(int sk);
char* itoa(int x);

int sendMsg(int sk,int size,char *filename);
void recvMsg(int sk,int *size,char *filename);

char* createSpace(char *data,int len);
char* getName(char *path);
int push(struct stack *s,int *top,DIR *data,int start);
DIR* pop(int *index,struct stack *s,int *top);
int deleteFile(char *path);
int repname(char *path,char *oldname,char *newname);
int createFile(char *path,char *fileNmae,int size);
int getSize(char *path,char *filename);

int createSocket(char *addr,int port,int sum);
int con(char *addr,int port);
int sendn(int sk,char *data,int type,int size);
void recvn(int sk,struct package *buf);
int sendSmallFile(int sk,int fd,int start,int end);
int sendBigFile(int sk,int fd,int start,int end);
int openFile(char *path);
void getStart(int num,int size,int *start,int *end,int count);
int recvFile(int sk,int fd,int count);


MYSQL* connectDatabase();
MYSQL_RES* query(MYSQL *database,char *sql);

void wRecordLock();
void wRecordUnlock();
void rRecordLock();
void rRecordUnlock();
void getInfo(int sk,struct nodeInfo *array,int scale);
void writeRecord(int *record,int count,int fd);
void readRecord(int *record,int count,int fd);
int linkSever(struct nodeInfo *node,int start,int index);
int download(int sk,int fd,int *record,char *buff,int num,int buffsize);
int fusion(char *filename,int sum,char *path);
int reconstruction(int *mark,int scale,int *record,int filesize);
void* getFile(void *args);
int reDonwn(int sk);
void init(int sk,int scal,char *pa);
void clear();
int downloads(int sk);

int putInfo(MYSQL *data,char *path,int scale,int sk);
int putfile(MYSQL *data,int sk,char *path,char *filename,int scale);
int handleFile(int sk);
