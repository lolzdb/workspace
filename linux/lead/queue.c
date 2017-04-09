/*************************************************************************
 > File Name: queue.c
 > Author: test*/
#include "queue.h"


void queue_init(queue *q){
	q->head=0;
	q->tail=0;
}

int push(queue *q,int fd){
	if((q->tail+1)%512==q->head)return 0;
	q->tail=(q->tail+1)%512;
	q->file[q->tail]=fd;
	return 1;
}

int pop(queue *q){
	int result;
	if((q->head+1)%512==q->tail)return 0;
	result=q->file[q->head];
	q->head=(q->head+1)%512;
	return result;
}
