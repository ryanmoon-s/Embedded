#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

int main(){
	pid_t pid, wpid;
	pid = fork();

	if(pid == 0){
		printf("---chile, my parent = %d , going to sleep 5s\n", getppid());
		sleep(5);
		printf("----------------chile died----------------");
	}else if(pid > 0){
		while(1){
			printf("---parent, pid = %d, my son = %d\n",getpid(), pid);
			sleep(1);
		}
	}else{
		perror("fork");
		exit(1);
	}
	
	return 0;
}
