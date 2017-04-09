source:=backups.c control.c database.c downfile.c sever.c transfer.c dirent.c
source1:=backups.c control.c ./client/downfile.c transfer.c ./client/dirent.c dirent.c client.c
source2:=backups.c control.c database.c downfile.c server1.c transfer.c dirent.c
source3:=backups.c control.c database.c downfile.c server2.c transfer.c dirent.c
server:$(source) 
	gcc -o server $^ -lpthread -lmysqlclient
clientc:$(source1)
	gcc -o clientc $^ -lpthread
server1:$(source2) 
	gcc -o server1 $^ -lpthread -lmysqlclient
server2:$(source3) 
	gcc -o server2 $^ -lpthread -lmysqlclient

