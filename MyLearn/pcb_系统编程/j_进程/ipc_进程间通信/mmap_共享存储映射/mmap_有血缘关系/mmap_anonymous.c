#include<stdio.h>
#include<stdlib.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
//匿名映射,不创建文件，利用映射在内存开辟空间完成父子通信

int main(){
	//不用打开文件

	//建立映射区 仅在Linux系统上有MAP_ANONYMOUS这个宏，其它类unix系统用dev/zero  /dev/null字符设备文件，也是匿名映射
	char *p = NULL;           // diffrience      位上MAP_ANONYMOUS    -1
	p = mmap(NULL, 4, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);//如果是PRIVATE，不共享
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
	int un = munmap(p, 4);//释放映射内存， 首地址、映射区大小
	if(un == -1){
		perror("munmap");
		exit(1);
	}

	return 0;
}
