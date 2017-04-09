#include "hall.h"

int main(){
    int client,sk;
    struct sockaddr sock;
    int len;
    struct pk x;
    sk=createSocket("127.0.0.1",10701,32);
    client=accept(sk,&sock,&len);
    handleFile(client);
    return 0;
}