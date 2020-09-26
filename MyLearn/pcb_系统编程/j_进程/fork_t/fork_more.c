#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
//getuid  实际用户id，geteuid  有效用户id，如sudo apt-get  ，有效用户是su
//父子间遵循读时共享，写时复制内存的原则，减少开销。慎用全局变量
//共享文件描述符(可以认为是下标对应的指针或包含文件的那个结构体,而不是0.1.2)、mmap建立的映射区

//不同处：进程Id,父进程id，fork返回值 ，程序运行时间，闹钟，未决信号集
//(0-3g几乎全部，4G部分)相同：全局变量，.data .txt 堆 栈 环境变量 用户id 宿主目录 进程工作目录 信号处理方式。。。

/*
gdb：在fork之前：(默认跟随父进程)
	set follow-fork-mode child ，跟踪子进程逻辑，父进程该输出还是输出，提前在循环因子i=3的时候设置断点，就跟踪第三个子进程
	set follow-fork-mode parent ，跟踪父进程
 */
int main(){
	//before
	printf("xxxxxxxxxxxx\n");


	/*
	   由一个进程变成两个进程，子进程接着fork的地方继续执行，上面的代码有，但不执行。
	   父进程返回子进程id，子进程成功返回0。
	 */

	//after
	pid_t pid;
	for(int i = 0; i < 5; i++){	
		pid = fork();
		if(pid == -1){
			perror("fork");
			exit(1);
		}else if(pid == 0){
			printf("i am %d child process, pid = %u, ppid = %u\n", i+1,  getpid(), getppid());
			break;
		}
		else{
		//	printf("i am a parent process, pid = %u, ppid = %u\n", getpid(), getppid());
			sleep(1);//打一个休眠一秒，防止子进程和父进程之间输出错乱,它们谁先占用cpu不确定,取决于内核调度算法。
		}
	}
	//所有子进程打印完了才return 0，不会出现bash解析在子进程的输出中间出现了
	//在中间出现的原因是a.out结束，bash出来，结果a.out又有子进程打印到终端。
	return 0;
}
