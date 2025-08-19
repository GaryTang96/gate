#	- 继续执行
#	@ 隐藏命令
#	$^ 引用全部
#	$< 引用第一个
hello.o: hello.c hello.h
	gcc -c hello.c -o hello.o

mian.o: hello.h main.c
	gcc -c main.c -o main.o

main: hello.o main.o
	gcc main.o hello.o -o main
	./main
	rm -rf *.o  *.i  main 

