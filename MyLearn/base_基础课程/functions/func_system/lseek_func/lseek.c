#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>

int main(){
	int fd = open("source.txt", O_RDWR);
	if(fd == -1){
		perror("open file");
		exit(1);
	}
	//获取文件长度
	int ret = lseek(fd, 0, SEEK_END);
	printf("file lentth:%d\n", ret);
	//文件拓展
	ret = lseek(fd, 2000, SEEK_END);
	printf("return value = %d\n",ret);
	//实现文件拓展，要在最后做一次写文件的操作，下载东西时，会先占用空间：空洞
	/*
		如果用truncate，就不用最后写入
		文件长度100.p2填20就截断，填300就拓展，同样是空洞
	*/
	write(fd, "a", 1);
	close(fd);
}
