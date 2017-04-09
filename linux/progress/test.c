#include<stdio.h>
#include<stdlib.h>

int test(void *argc){
    void **x=argc;
    int *a=(int *)x[0];
    printf("%d\n",*a);
    return 1;
}

int main(){
    void *argc[2];
    int a=1;
    argc[0]=(void *)&a;
    test(argc);
    return 0;
}