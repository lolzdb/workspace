#include "hall.h"



MYSQL* connectDatabase(){
    char *server="localhost";
    char *user="root";
    char *password="123";
    char *data="cloude";
    MYSQL* database=mysql_init(NULL);
    if(!mysql_real_connect(database,server,user,password,data,0,NULL,0)){
        printf("mysql error %s\n",mysql_error(database));
        return 0;
    }
  return database;
}

MYSQL_RES* query(MYSQL *database,char *sql){
    int count=mysql_query(database,sql);
    MYSQL_RES *result;
    if(count!=0){
        printf("sql=%s\n query error %s\n",sql,mysql_error(database));
    }
    result=mysql_store_result(database);
    if(result==NULL){
        printf("sql=%s\n query error %s\n",sql,mysql_error(database));
    }
    return result;
}