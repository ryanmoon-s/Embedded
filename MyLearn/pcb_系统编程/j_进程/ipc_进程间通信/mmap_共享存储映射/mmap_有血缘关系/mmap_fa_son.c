#include<stdio.h>
#include<stdlib.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>

int main(){
	//打开文件
	int fd = open("temp", O_RDWR|O_CREAT|O_TRUNC, 0644);
	if(fd < 0){
		perror("open");
		exit(1);
	}
	unlink("temp");//删除目录项，持有该文件的进程结束后文件将会被删除，临时文件
	//建立映射区
	int len = ftruncate(fd, 4);   //超过原文件大小则拓展，小于则截取，由此知道文件大小，单位字节
	if(len == -1){
		perror("ftruncate");
		exit(1);
	}
	char *p = NULL; 
	p = mmap(NULL, 4, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);//如果是PRIVATE，不共享
	if(p == MAP_FAILED){  //失败返回MAP_FAILED
		perror("mmap");
		exit(1);
	}
	//操作
	pid_t pid = fork();
	if(pid == 0){
		strcpy(p, "fff");	
	}else if(pid > 0){
		sleep(1);
		printf("%s\n", p);
		wait(NULL);
	}else{
		perror("fork");
		exit(1);
	}
	//释放
	close(fd);
	int un = munmap(p, 4);//释放映射内存， 首地址、映射区大小
	if(un == -1){
		perror("munmap");
		exit(1);
	}

	return 0;
}
