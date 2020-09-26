#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#if 0
	man 中的 other architectures，半双工，单身
	函数调用成功返回r/w两个文件描述符。无需open，但需手动close。规定：fd[0] → r； fd[1] → w
	向管道文件读写数据其实是在读写内核缓冲区,管道实为内核使用环形队列机制，借助内核缓冲区(4k)实现
	和环形队列一样的机制，出队列了就没了，不可反复读取。只能一边进，一边出
#endif
#if 0
	左写，右读
	1.开 开 ，左未写入数据，右再次read会阻塞等待数据，所以保证始终能读到数据。  左写满了，阻塞，正常写完左返回字节数
	2.开 关 ，左收到信号SIGPIPE，异常终止，也可捕捉
	3.关 开 ，右返回0，类似文件读时读完了
#endif
int main(){
	int fd[2];
	int ret = pipe(fd);
	char buff1[100] = "hello my dear\n";
	char buff2[100];
	if(ret == -1){
		perror("create pipe");
		exit(1);
	}
	pid_t pid;
	pid = fork();
	if(pid == 0){
		//子进程关闭写
		close(fd[1]);
		//子进程读
		int rea = read(fd[0], buff2, sizeof(buff2));
		if(rea == -1){
			perror("read");
			exit(1);
		}
		close(fd[0]);
		//打印到屏幕
		int w = write(1, buff2, sizeof(buff2));
		if(w == -1){
			perror("printf");
			exit(1);
		}
	}else if(pid > 0){
		//父进程关闭读
		close(fd[0]);
		//父进程写
		int wir = write(fd[1], buff1, sizeof(buff1));
		if(wir == -1){
			perror("write");
			exit(1);
		}
		close(fd[1]);
		wait(NULL);
	}else{
		perror("fork");
		exit(1);
	}
}
