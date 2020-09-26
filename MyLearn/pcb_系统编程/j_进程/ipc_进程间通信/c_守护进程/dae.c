#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>

void set_err(char* str){
	perror(str);
	exit(1);
}
void creat_dae();
//出错暂时不加
int main(){
	creat_dae();
	pid_t pid1;
	int fd = open("shun_data.txt", O_CREAT|O_WRONLY, 0644);
	while(1){
		pid1 = fork();
		if(pid1 == 0){
			dup2(fd, STDOUT_FILENO);
			execlp("date", "date", NULL);
		}
		sleep(6);
	}

	return 0;
}

void creat_dae(){
	pid_t pid;
	//1
	pid = fork();
	if(pid > 0){
		exit(1);//要用exit退出程序，return只是退出函数
	}
	//2
	setsid();
	//3
	int re = chdir("/home/itcas/");
	if(re == -1)
		set_err("chdir");
	//4
	umask(0022);    //644  022
	//5
	close(STDOUT_FILENO);
	open("/dev/null", O_RDWR);//占用0号描述符 这样不会让3被占用，下次打开文件就在3里面
	dup2(0, STDOUT_FILENO);
	dup2(0, STDERR_FILENO);
	//6
	//一般在这里面注册一个信号捕捉函数接收信号-释放资源关闭锁自杀，在这里创是保证守护进程已经搭建好
}
