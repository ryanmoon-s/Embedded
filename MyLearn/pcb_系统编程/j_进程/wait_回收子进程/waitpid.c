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
		//p3设为0，阻塞的方式，成功返回子进程Id，失败-1，参数p2为int *传出参数->子进程状态，NULL则不获取
		//p3设为WNOHANG，非阻塞,轮循方式，一会回去看看结束了就回收，返回0->子程序正在运行。 
		//pid>0指定子进程id,-1回收任意子进程,0一个组的任意进程, <-1 回收指定进程组的任意子进程(-1005  杀死组id为1005的所有，ps ajx查看)
		do{
			wpid = waitpid(pid, NULL, 1); // 0阻塞，1WNOHANG不阻塞
		}while(wpid == 0);
#if 0
		回收所有子进程

		while(waitpid(-1, NULL, 0));  有错，自己按个数回收

		do{   // WNOHANG时，子进程正在运行，未回收，返回0，       阻塞时回收失败回返-1
			wpid = waitpid(-1, NULL, WNOHANG);
			int count = 0;
			if(pid > 0){
				count++;
			}
			sleep(1);
		}while(count == n);
#endif
		if(wpid == -1){ 		
			perror("wait error");
			exit(1);
		}
		printf("finish\n");
	}else{
		perror("fork");
		exit(1);
	}
	
	return 0;
}
