#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
//dup2:(old,new)，如果new和old不一样new又打开了，则解除new的绑定再赋值，    如果一样，则不动。

int main(){
	int fd=open("a", O_RDWR);
	if(fd == -1){
		perror("open");
		exit(1);
	}
	printf("fd = %d\n", fd);
	int ret = dup(fd); //ret 和 fd指向同一个文件
	if(ret == -1){
		perror("dup");
		exit(1);
	}
	printf("ret = %d\n", ret);
	char buff[] = "hello " ;   
	char buff1[] = "world";   //指针已经后移，接着上面输出 
	write(fd, buff, sizeof(buff));
	write(ret, buff1, sizeof(buff1));
	close(fd);
	close(ret);
}
