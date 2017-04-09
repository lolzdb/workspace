#include<stdio.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>

typedef struct stack{
    int start;
    DIR *data;
}stack;


char* createSpace(char *data,int len);
char* getName(char *path);
int push(stack *s,int *top,DIR *data,int start);
DIR* pop(int *index,stack *s,int *top);
int deleteFile(char *path);
int repname(char *path,char *oldname,char *newname);
int createFile(char *path,char *fileNmae,int size);
int getSize(char *path,char *filename);