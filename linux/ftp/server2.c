#include "hall.h"

int main(){
    int client,sk;
    struct sockaddr sock;
    int len;
    sk=createSocket("127.0.0.1", 10700,32);
    client=accept(sk,&sock,&len);
    handleFile(client);
    return 0;
}