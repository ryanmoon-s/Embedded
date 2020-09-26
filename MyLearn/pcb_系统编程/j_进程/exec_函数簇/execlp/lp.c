#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
//l->list命令行参数列表   p->path ，搜索时使用path环境变量 v 使用命令行参数数组， e使用环境变量数组，设置新程序的环境变量
//l v lp vp le ve        (l/v e/p)   前者必须有，后者可以没有，不需要环境变量 

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
		//子进程,p1 可执行文件名，p2 argv[0], p3  argv[1]...(如果是命令行参数以NULL结束)
		execlp("ls", "ls", "-l",  NULL);
	}
	return 0;
}
