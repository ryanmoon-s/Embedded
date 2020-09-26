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
		printf("----------------chile died----------------\n");
	}else if(pid > 0){
		//成功返回子进程Id，失败-1，参数为int *传出参数->子进程状态，NULL则不获取
		wpid = wait(NULL);{//阻塞等待回收子进程，回收了才做下面的事
			if(wpid == -1){//只回收一个子进程，哪个先结束，先回收哪个
				perror("wait error");
				exit(1);
			}
		}
#if 0
		回收全部子进程
		while(wait(NULL));  有错，自己按个数回收
#endif


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
