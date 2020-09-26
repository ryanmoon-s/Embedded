#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>

int main(){
	//before
	printf("xxxxxxxxxxxx\n");


	/*
	   由一个进程变成两个进程，子进程接着fork的地方继续执行，都有fork返回值 ，上面的代码有，但不执行。
	   父进程返回子进程id，子进程成功返回0。
	*/

	//after
	pid_t pid;
	pid = fork();
	if(pid == -1){
		perror("fork");
		exit(1);
	}else if(pid == 0){
		printf("i am a child process, pid = %u, ppid = %u\n", getpid(), getppid());
	}
	else{
		printf("i am a parent process, pid = %u, ppid = %u\n", getpid(), getppid());
		sleep(1);
	}
	return 0;
}
