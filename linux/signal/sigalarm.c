#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<errno.h>
#include<unistd.h>
#include<sys/un.h>
#include<fcntl.h>
#include<sys/types.h>
#include<stddef.h>
#include<sys/stat.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<signal.h>

union semun{
    short val;
};

int sk[2];

char *getshare(int choice){
    key_t key=ftok("../progress/test",1);
    int sm;
    char *addr;
    if(choice==1)sm=shmget(key,4096,IPC_CREAT|IPC_EXCL|0666);
    else sm=shmget(key,4096,IPC_CREAT|0666);
    addr=shmat(sm,NULL,0);
    if(((long)addr)==-1){
        perror("link ");
        return NULL;
    }
    return addr;
}

int seminit(int choice){
     key_t key=ftok("../progress/test.c",1);
     int sm,len;
     union semun val;
     if(choice==1)sm=semget(key,1,IPC_CREAT|IPC_EXCL|0666);
     else sm=semget(key,1,IPC_CREAT|0666);
     if(sm==-1){
         perror("get sem");
         return 0;
     }
     val.val=1;
     len=semctl(sm,0,SETVAL,val);
     if(len==-1){
         perror("get sem");
         return 0;
     }
     return sm;
}

int fifo(char *path,int mode){
    int fd;
     printf("1------path=%s\n",path);
    if(mkfifo(path,0666)<0&&errno!=EEXIST){
        perror("sysytem error");
        return 0;
    }
   printf("2------path=%s  %d\n",path,mode);
    if((fd=open(path,mode))<0){
        perror("openfile error");
        return 0;
    }
    printf("3------path=%s\n",path);
    return fd;
}

int lock(int se,int pv){
    struct sembuf op;
    int count;
    op.sem_num=0;
    op.sem_op=pv;
    op.sem_flg=SEM_UNDO;
    count=semop(se,&op,1);
    if(count==-1)return -1;
    return 1;
}

void sighandler(int num){
    send(sk[1],&num,sizeof(num),0);
}



int addsig(int sig){
    struct sigaction act;
    act.sa_handler=sighandler;
    act.sa_flags=SA_RESTART;
    sigfillset(&act.sa_mask);
    sigdelset(&act.sa_mask,SIGINT);
    sigaction(SIGINT,&act,NULL);
    return 1;
}


int setnoblock(int fd){
    int val;
    val=fcntl(fd,F_GETFL,0);
    val=val|O_NONBLOCK;
    fcntl(fd,F_SETFL);
    return 1;
}

void sigact(int num){
    printf("recv sig\n");
}

int chile(){
    char *buff=getshare(0);
    int se=seminit(0),*mk;
    //signal(SIGALRM,sigact);
    mk=(int *)buff;
    buff+=4;
    *mk=1;
    while(1){
        pause();
        lock(se,-1);
        buff[*mk]=0;
        printf("buff=%s\n",buff);
        lock(se,1);
        if(*mk==0)break;
    }
    shmdt(buff-4);
    semctl(se,0,IPC_RMID,NULL);
    printf("chile finish\n");
    return 1;
}

int parent(int pid){
    char *buff=getshare(0);
    int se=seminit(0);
    int read1=fifo("read",O_RDONLY);
    int write1=fifo("write",O_WRONLY);
    int count,len,*mk=(int *)buff;
    char ms[512];
    struct timeval time;
    fd_set r;
    socketpair(AF_UNIX,SOCK_STREAM,0,sk);
    addsig(SIGINT);
    time.tv_sec=1;
    time.tv_usec=0;
    buff=buff+4;
  //  setnoblock(read1);
  //  setnoblock(write1);
    while(1){
        FD_ZERO(&r);
        FD_SET(read1,&r);
        FD_SET(sk[0],&r);
        FD_SET(STDIN_FILENO,&r);
        count=select(64,&r,NULL,NULL,&time);
        if(count==0)continue;
        if(FD_ISSET(read1,&r)){
            lock(se,-1);
            count=read(read1,buff,1024);
            *mk=count;
            lock(se,1);
            printf("count=%d\n",count);
            kill(pid,SIGALRM);
            if(count==0){
                printf("finish\n");
                *mk=0;
                break;
            }
        }
        if(FD_ISSET(STDIN_FILENO,&r)){
            fgets(ms,512,stdin);
            write(write1,ms,strlen(ms));
        }
        if(FD_ISSET(sk[0],&r)){
            count=recv(sk[0],mk,4,0);
            printf("count=%d  recv=%d\n",count,*mk);
            break;
        }
    }
    close(read1);
    close(write1);
    sleep(2);
    shmdt(buff-4);
    semctl(se,0,IPC_RMID,NULL);
    return 0;
}

int main(){
    pid_t pid;
    if((pid=fork())<0){
        perror("pid ");
        return 0;
    }
    if(pid==0){
        chile();
    }else{
        parent(pid);
        wait(NULL);
    } 
}