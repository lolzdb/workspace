//#include "head/dirent.h"
#include "hall.h"

char* createSpace(char *data,int len){
    char *ndata=(char *)malloc(sizeof(char)*(len+512));
    for(int i=0;i<len;i++)
        ndata[i]=data[i];
    free(data);
    return ndata;
}

char* getName(char *path){
    struct stat fileInfo;
    DIR *dir;
    struct dirent *dirInfo;
    char *info=(char *)malloc(sizeof(char)*512);
    int len=512,index=0,nlen;
    stat(path,&fileInfo);
    if(S_ISREG(fileInfo.st_mode))return 0;
    dir=opendir(path);
    while((dirInfo=readdir(dir))){
        if(strcmp(dirInfo->d_name,".")==0||strcmp(dirInfo->d_name,"..")==0)continue;
        if(dirInfo->d_type==4){
            strcat(info,"D ");
            index+=2;
        }
        nlen=strlen(dirInfo->d_name)+index;
        if(nlen>=len){
            len+=512;
            info=createSpace(info,len);
        }
        for(int i=index,j=0;i<nlen;i++,j++){
            info[i]=dirInfo->d_name[j];
        }
        index=nlen;
        info[index++]=1;
    }
    info[--index]=0;
    return info;
}

int push(struct stack *s,int *top,DIR *data,int start){
    s[++(*top)].data=data;
    s[*top].start=start;
    return 0;
}

DIR* pop(int *index,struct stack *s,int *top){
    *index=s[*top].start;
    return s[(*top)--].data;
}

int deleteFile(char *path){
    char *filepath=(char *)malloc(sizeof(char)*1024);
    struct stack *s=(struct stack *)malloc(sizeof(struct stack)*256);
    int index=strlen(path),top=0;
    DIR *curDir;
    struct dirent *dir;
    s[top].data=NULL;
    s[top].start=0;
    strcpy(filepath,path);
    curDir=opendir(path);
    while(curDir!=NULL){
        while((dir=readdir(curDir))!=NULL){
            if(dir->d_type==4){
                if(strcmp(dir->d_name,".")==0||strcmp(dir->d_name,"..")==0)continue;
                push(s,&top,curDir,index);
                filepath[index]='/';
                filepath[index+1]=0;
                strcat(filepath,dir->d_name);
                printf("2 filepath=%s\n",filepath);
                index+=1+strlen(dir->d_name);
                filepath[index]=0;
                curDir=opendir(filepath);
                printf("filepath=%s\n",filepath);
            }else{
                filepath[index]='/';
                filepath[index+1]=0;
                strcat(filepath,dir->d_name);
                remove(filepath);
                filepath[index]=0;
            }
        }
        remove(filepath);
        curDir=pop(&index,s,&top);
        filepath[index]=0;
    }
    free(filepath);
    free(s);
    return 0;
}

int repname(char *path,char *oldname,char *newname){
    char path1[1024];
    char path2[1024];
    strcpy(path1,path);
    strcpy(path2,path);
    strcat(path1,"/");
    strcat(path2,"/");
    strcat(path1,oldname);
    strcat(path2,newname);
    rename(path1,path2);
    return 0;
}

int createFile(char *path,char *fileNmae,int size){
    char path1[1024];
    strcpy(path1,path);
    strcat(path1,"/");
    strcat(path1,fileNmae);
    creat(path1,0600);
    truncate(path1,size);
    return 0;
}

int getSize(char *path,char *filename){
    struct stat buf;
    char path1[1024];
    strcpy(path1,path);
    strcat(path1,"/");
    strcat(path1,filename);
    stat(path1,&buf);
    return buf.st_size;
}