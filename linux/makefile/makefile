g=gcc -c
target=main.o hello.o
main:$(target) test.h
	gcc -o main hello.o main.o
main.o:main.c
	$(g) main.c
hello.o:hello.c
	$(g) hello.c
.PHONY:clean1 clean2

clean1:
	touch a.txt
clean2:
	touch b.txt
clean3:
	touch c.txt
