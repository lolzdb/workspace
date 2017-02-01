/*************************************************************************
 > File Name: epoll.c
 > Author: test*/
#include<sys/epoll.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#define MAXLINK 512

int sk,ep;

void *recvms(void *argc){
	struct epoll_event earray[MAXLINK];
	int count=0;
	int len=0;
	char data[256];
	while(1){
		count=epoll_wait(ep,earray,MAXLINK,100);
		for(int i=0;i<count;i++){
			len=recv(earray[i].data.fd,data,256,0);
			data[len]=0;
			printf("len=%d   %d  say  %s",len,earray[i].data.fd,data);
		}
	}
}

int main(){
	int ck;
	struct sockaddr_in sever,client;
	struct epoll_event event;
	int count=0,*earray;
	int len=sizeof(sever);
	pthread_t rc;
	ep=epoll_create(1024);
	perror("c epoll ");
	sk=socket(AF_INET,SOCK_STREAM,0);
	if(sk<0){
		perror("create socket");
		exit(1);
	}
	memset(&sever,0,sizeof(sever));
	sever.sin_family=AF_INET;
	sever.sin_port=htons(10701);
	sever.sin_addr.s_addr=inet_addr("127.0.0.1");
	if(bind(sk,(struct sockaddr *)&sever,len)<0){
		perror("bind ");
		exit(2);
	}
	listen(sk,MAXLINK);
	pthread_create(&rc,NULL,recvms,NULL);
	while(1){
		ck=accept(sk,(struct sockaddr *)&client,&len);
		if(ck<0){
			perror("accept ");
			exit(3);
		}
		event.events=EPOLLIN;
		event.data.fd=ck;
		epoll_ctl(ep,EPOLL_CTL_ADD,ck,&event);
		perror("epoll ");
	}

}
