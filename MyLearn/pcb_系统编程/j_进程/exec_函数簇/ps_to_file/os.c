#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>

int main(){
	int fd;
	fd = open("psout.txt", O_WRONLY| O_CREAT| O_TRUNC, 0644);

	if(fd == -1){
		perror("open");
		exit(1);
	}
	dup2(fd, STDOUT_FILENO); //最终都指向前者，而stdout对应的是输出 ，所以输出到fd对应的文件中
	execlp("ps", "ps", "aux", NULL);//所有exec只在有错时返回值 -1，成功不返回值
	//此进程已经被 execlp替换，execlp执行完成后直接退出，下面的语句不起作用，此处依赖于隐式回收
	//close(fd);

	return 0;
}
