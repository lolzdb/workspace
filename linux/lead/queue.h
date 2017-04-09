/*************************************************************************
 > File Name: queue.h
 > Author: test*/

typedef struct w{
	int file[512];
	int head,tail;
}queue;

void queue_init(queue *q);
int push(queue *q,int fd);
int pop(queue *q);
