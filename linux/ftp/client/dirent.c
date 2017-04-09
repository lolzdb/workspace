#include<stdio.h>
#include<string.h>

struct package{
    int sum;
    int mark;
    char data[1024];
};

int printfDir(char *dirInfo){
    while(*dirInfo!=0){
        if(*dirInfo==1){
            printf("    ");
             dirInfo++;
            continue;
        }
        printf("%c",*dirInfo);
        dirInfo++;
    }
    return 0;
}


