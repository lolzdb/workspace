#include "hall.h"


int main(){
    int client,sk;
    struct sockaddr sock;
    int len;
    MYSQL *data=connectDatabase();
    sk=createSocket("127.0.0.1",10702,32);
    client=accept(sk,&sock,&len);
    putfile(data,client,"/home/xws/workspace/linux/ftp","a.txt",2);
    mysql_close(data);
    return 0;
}

/*


    int fd=openFile("/home/xws/workspace/linux/ftp/a.txt");
    int sock=createSocket("127.0.0.1",10701,32);
    char data[1025];
    int n;
    struct sockaddr addr;
    int sk=accept(sock,&addr,&n);
    while((n=read(fd,data,1024))!=0){
        data[n]=0;
        sendn(sk,data,0,n);
        printf("%s",data);
    }
    printf("\n");
    close(sk);
    return 0;
*/