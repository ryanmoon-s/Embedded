#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
//l->命令行参数列表      不带p  以路径来执行，可用来加载自定义的程序

int main(){
	pid_t pid;
	pid = fork();
	if(pid == -1){
		perror("fork");
		exit(1);
	}else if(pid > 0){
		//父进程
		printf("i am parent\n");
		sleep(1);
	}else{
		//子进程,p1 路径名，p2 argv[0], p3  argv[1]...(如果是命令行参数以NULL结束)
		execl("/bin/ls", "ls", "-l",  NULL);
		//可以相对路径 ./
	}
	return 0;
}
