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
void add(int num){
    printf("get signal=%d\n",num);
}

int main(){
    pid_t p;
    p=fork();
    if(p==0){
        pause();
        printf("get signal\n");
    }else{
        int a=0;
        sleep(3);
        kill(p,SIGALRM);
        perror("kill ");
        wait(&a);
        if(WIFSIGNALED(a))printf("%d",a);
        printf("finish  all\n");
    }
    return 0;
}