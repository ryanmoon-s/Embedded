#include<stdio.h>
#include<stdlib.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>

void set_err(char* str);//错误打印
void writes(int* jump, char* p, int fd, int i);//子进程写

int main(){
	int i;
	long int fsize;
	pid_t pid;
	int assign[6];
	//打开文件
	int fd1 = open("123.txt", O_RDONLY);//源文件
	int fd2 = open("321.txt", O_WRONLY|O_CREAT|O_TRUNC, 0644);//目标文件，子进程共享此描述符，都写完父进程再关闭。
	//获取源文件大小
	struct stat target;
	int ret = fstat(fd1, &target);
	if(ret == -1)
		set_err("stat");
	fsize = (long int)target.st_size;
	//拓展目标文件
	int ft = ftruncate(fd2, fsize);
	if(ft == -1)
		set_err("ftruncate");
	//建立映射区
	char* first = NULL;
	first = mmap(NULL, fsize, PROT_READ, MAP_PRIVATE, fd1, 0);
	if(first == MAP_FAILED)
		set_err("mmap");
	char *p = first;
	close(fd1);
	//任务分配，装的每个进程需要跳过的字节数
	int temp = fsize/5;
	for(int j = 0; j < 5; j++){
		assign[j] = j*temp;
	}
	assign[5] = fsize - temp * 4;//数组最后一格存最后一个进程需要写的字节数
	//创建子进程 5 个
	for(i = 0; i < 5; i++){
		if(pid = fork() == 0)
			break;
		if(pid == -1)
			set_err("fork");
	}
	//进程处理
	switch (i)
	{
	case 0:
		writes(assign, p, fd2, i);
		break;
	case 1:
		writes(assign, p, fd2, i);
		break;
	case 2:
		writes(assign, p, fd2, i);
		break;
	case 3:
		writes(assign, p, fd2, i);
		break;
	case 4:
		writes(assign, p, fd2, i);
		break;
	default: 
		//父进程回收
		for(int j = 0; j < 5; j++)
			wait(NULL);
		close(fd2);
		//释放Mmp
		int unm = munmap(first, fsize);
		if(unm == -1)
			set_err("munmap");
	}
	return 0;
}


void set_err(char* str){
	perror(str);
	exit(1);
}

void writes(int* jump, char *p, int fd, int i){
		p += jump[i];
		int re = lseek(fd, jump[i], SEEK_SET);
		if(re == -1)
			set_err("lseek");
	if(i < 4){
		int wr = write(fd, p, jump[1]);
		if(wr == -1)
			set_err("write");
	}else{		
		int wr1 = write(fd, p, jump[5]);
		if(wr1 == -1)
		set_err("write");
	}
}
