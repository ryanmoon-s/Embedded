#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#if 0
	first in first out，又名：有名管道，半双工(单向)，
	每个FIFO有一个路径名与之关联，所以可以可无亲情关系。
	int mkfifo 成功返回0，出错返回-1。pathname是路径名(文件)，是FIFO的名字，mode指定文件的权位。 (mode & ~umask).
	在创建FIFO后，必须打开来读 或者 打开来写，不能打开来既读既写（因为FIFO是半双工）
#endif

//总结:fifo制作的是一个包含路径的文件，不是路径。不能先touch，要在代码中创建
//和正常的打开文件一样操作，以FIFO的形式存在文件系统中，它的内容放在内存中，一个只读打开，一个只写打开，就可以传输数据了。
//当使用 FIFO 的进程退出后，FIFO 文件将继续保存在文件系统中以便以后使用。
int main(){
#define Fifo "./ffifo"
//	mkfifo(Fifo, 0664);
	char buff[20];
	int fd1 = open(Fifo, O_RDONLY);
	int re = read(fd1, buff, sizeof(buff));	
	if(re == -1){
		perror("write");
		exit(1);
	}
	printf("%s\n",buff);
	return 0;
}
