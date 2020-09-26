#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#if 0
	man 中的 other architectures
	函数调用成功返回r/w两个文件描述符。无需open，但需手动close。规定：fd[0] → r； fd[1] → w
	向管道文件读写数据其实是在读写内核缓冲区,管道实为内核使用环形队列机制，借助内核缓冲区(4k)实现
#endif
#if 0
	左写，右读
	1.开 开 ，左未写入数据，右再次read会阻塞等待数据，所以保证始终能读到数据。  左写满了，阻塞，正常写完左返回字节数
	2.开 关 ，左收到信号SIGPIPE，异常终止，也可捕捉
	3.关 开 ，右返回0，类似文件读时读完了
#endif
int main(){
	int i;
	int fd[2];
	int ret = pipe(fd);
	char buff1[100] = "hello my dear\n";
	char buff2[100];
	if(ret == -1){
		perror("create pipe");
		exit(1);
	}
	pid_t pid;
	for(i = 0; i < 2; i++){
		if((pid = fork()) == 0){
			break;
		}	
	}
	if(i == 0){
		//子进程1，关闭读，自己写
		close(fd[0]);
		int wr = write(fd[1], buff1, sizeof(buff1));
		if(wr == -1){
			perror("write");
			exit(1);
		}
		close(fd[1]);
	}else if(i == 1){
		//子进程2，关闭写，自己读
		close(fd[1]);
		int re = read(fd[0], buff2, sizeof(buff2));
		if(re == -1){
			perror("read");
			exit(1);
		}
		write(1, buff2, sizeof(buff2));
		close(fd[0]);
	}else{
		//父进程，关闭所有接口
		close(fd[0]);
		close(fd[1]);
		wait(NULL);
		wait(NULL);
	}
	return 0;
}
