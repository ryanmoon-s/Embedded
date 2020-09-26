#include<stdio.h>
#include<signal.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
//10 SIGUSR1， 12 SIGUSR2 系统留给用户用的，默认终止进程
//全局变量异步IO：两个地方在用全局变量，没协商。  方法一：取消全局变量，直接在捕捉里面发信号
//方法二：加锁

int n = 0, flag = 0;
void child_catch(int x){
	printf("child pid = %d\t, n = %d\n", getpid(), n);
	n+=2;
	flag = 1;  //数数完成
//	sleep(1);   按道理去了效果也一样   停1s，让信号发送者能退出if语句来捕捉信号
}

void parent_catch(int x){
	printf("parent pid = %d\t, n = %d\n", getpid(), n);
	n += 2;
	flag = 1;  //数数完成
//	sleep(1);
}

void set_err(char* str){
	perror(str);
	exit(1);
}

int main(){
	struct sigaction act;
	pid_t pid;
	if((pid = fork()) == -1)
		set_err("fork");
	if(pid > 0){
		n = 1;
		//注册信号捕捉
		act.sa_handler = parent_catch;
		sigemptyset(&act.sa_mask);
		act.sa_flags = 0;
		int re = sigaction(SIGUSR2, &act, NULL);
		//启动动作
		sleep(1);
		parent_catch(1);
		//信号发送，信号捕捉函数处理完成
		while(1){
			    //按道理父应该在if外面捕捉信号->flag = 1->进入if发信号，但是在kill后失去了cpu，没有来到外面，就在里面捕捉并把flag = 1，再顺序flag = 0，没有发送信号，所以over
			if(flag == 1){
				kill(pid, SIGUSR1);
				//----------lose cpu---------子进程->捕捉->发信号->
				//----------get cpu----------父进程捕捉信号->flag = 1->
				flag = 0; //  父进程flag = 0
			}
				//---------------------------父进程flag = 0  卡在这了，父没给子发信号，子也不会给父发信号
		}
	}
	if(pid == 0){
		//注册信号捕捉
		n = 2;
		act.sa_handler = child_catch;
		sigemptyset(&act.sa_mask);
		act.sa_flags = 0;
		int re = sigaction(SIGUSR1, &act, NULL);
		//信号发送，信号捕捉函数处理完成
		while(1){
			if(flag == 1){
				kill(getppid(), SIGUSR2);
				//------------lose cpu-------
				flag = 0;
			}
		}
	}
	return 0;
}



