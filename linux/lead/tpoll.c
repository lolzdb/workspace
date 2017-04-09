/*************************************************************************
 > File Name: tpoll.c
 > Author: test*/

#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>
#include "queue.h"
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/epoll.h>

#define MAXLIMIT 8

#define MAXLINK 512



pthread_mutex_t lock;

pthread_cond_t cq;

int list;
int epoll;
struct sockaddr_in *sever;
queue q;
int fdset[MAXLINK];
int scount;
pthread_t parray[MAXLIMIT];

int delete(int fd){
	int i;
	for(i=0;i<scount;scount++){
		if(fdset[i]==fd)break;
	}
	i++;
	for(;i<scount;i++){
		fdset[i-1]=fdset[i];
	}
	return 1;
}

void *worker(void *argc){
    char data[256];
	int len,fd;
while(1){
	pthread_mutex_lock(&lock);
	pthread_cond_wait(&cq,&lock);
	fd=pop(&q);
	pthread_mutex_unlock(&lock);
	if(fd==0){
		continue;
	}
	len=recv(fd,data,256,0);
	data[len]=0;
	if(len==0||len==-1){
		struct epoll_event ee;
	    ee.events=EPOLLIN;
		ee.data.fd=fd;
		epoll_ctl(epoll,EPOLL_CTL_DEL,fd,&ee);
		scount--;
		perror("delete ");
		close(fd);
		delete(fd);
		continue;
	}
	data[len]=0;
	for(int i=0;i<scount;i++){
		if(fd!=fdset[i]){
			send(fdset[i],data,len,0);
		}
	}
  }
}

void *deliverMs(void *argc){
	int fd,count=0,max=512,i;
	struct epoll_event earray[512];
	while(1){
      count=epoll_wait(epoll,earray,max,300);
	  for(i=0;i<count;i++){
		fd=earray[i].data.fd;
		if(push(&q,fd)>0){
			pthread_cond_signal(&cq);
		}
	  }
	}
}

void ep(){
	int ck;
	pthread_t deliver;
	struct epoll_event add;
	int len=sizeof(*sever);
	pthread_create(&deliver,NULL,deliverMs,NULL);
	while(1){
		ck=accept(list,(struct sockaddr *)sever,&len);
		add.events=EPOLLIN;
		add.data.fd=ck;
		fdset[scount]=ck;
		scount++;
		epoll_ctl(epoll,EPOLL_CTL_ADD,ck,&add);
	}
}

int lead(){}


int init(int flags,struct sockaddr_in *info,int socket){
	int result=0,len=sizeof(*sever);
	list=socket;
	sever=info;
	epoll=epoll_create(MAXLINK);
	pthread_mutex_init(&lock,NULL);
	pthread_cond_init(&cq,NULL);
	queue_init(&q);
	pthread_t array[MAXLIMIT];
	for(int i=0;i<MAXLIMIT;i++){
		result=pthread_create(&parray[i],NULL,worker,NULL);
		if(result<0){
			perror("init ");
			exit(1);
		}
	}
	if(flags==1)lead();
	else ep();
}

int main(){
	int sk,len;
	struct sockaddr_in sever;
	len=sizeof(sever);
	sk=socket(AF_INET,SOCK_STREAM,0);
	memset(&sever,0,len);
	sever.sin_family=AF_INET;
	sever.sin_port=htons(10701);
	sever.sin_addr.s_addr=inet_addr("127.0.0.1");
   if(bind(sk,(struct sockaddr *)&sever,len)<0){
	   perror("bind ");
	   exit(1);
   }
   listen(sk,MAXLINK);
   init(2,&sever,sk);
}
