#include "hall.h"

int main(){
    struct pk x,*d;
    char p[1024];
    x.a=1;
    x.b=2;
    x.c=3;
    strcpy(x.d,"1234");
    memcpy(p,&x,sizeof(struct pk));
    d=(struct pk*)p;
    printf("%d  %d   %d    %s\n",d->a,d->b,d->c,d->d);
    return 0;
}