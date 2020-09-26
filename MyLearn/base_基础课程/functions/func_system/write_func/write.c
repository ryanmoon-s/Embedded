#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>

//write(1, buf, len)   写到屏幕上
//0 输入 默认-> stdin    1输入 默认->stdout   2出错

int main(){
	//打开已有的文件
	int fd = open("source.txt", O_RDONLY);
	if(fd == 1){
		perror("open file");
		exit(1);
	}
	//创建新文件
	int fd1 = open("target.txt", O_CREAT | O_WRONLY ,0664);
	if(fd1 == 1){
		perror("open file");
		exit(1);
	}
	//读文件
	char buf[200] = {0};
	int count = read(fd, buf, sizeof(buf));
	if(count == -1){
		perror("read file");
		exit(1);
	}	
	while(count){
		//将读出的数据写入到另一个文件中
		int ret = write(fd1, buf, count);
		printf("write bytes %d\n",ret);
		count = read(fd, buf, sizeof(buf));
	}
	//关闭文件
	close(fd);
	close(fd1);


}
