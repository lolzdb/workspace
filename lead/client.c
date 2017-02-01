/*************************************************************************
 > File Name: client.c
 > Author: test*/
#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>

int s;
struct sockaddr_in sever;

void *receive(void *a){
	char data[1024];
	int len;
	while(1){
		len=recv(s,data,256,0);
		data[len]=0;
		if(len==1){
			close(s);
			return NULL;
		}
		printf("%s",data);
	}
}

int main(){
	int len=sizeof(sever);
	char data[1024];
	pthread_t pt;
	s=socket(AF_INET,SOCK_STREAM,0);
	sever.sin_family=AF_INET;
	sever.sin_port=htons(10701);
	sever.sin_addr.s_addr=inet_addr("127.0.0.1");
	pthread_create(&pt,NULL,receive,NULL);
	if(connect(s,(struct sockaddr *)&sever,len)<0){
		perror("connect ");
		exit(1);
	}
	while(1){
		fgets(data,1024,stdin);
		len=strlen(data);
		data[len]=0;
		send(s,data,len+1,0);
	}
	return 0;
}
