#include<stdio.h>
#include<stdlib.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

int main(){
	//打开文件
	int fd = open("test.txt", O_RDWR);
	if(fd < 0){
		perror("open");
		exit(1);
	}
	//建立映射区
	int len = ftruncate(fd, 4);   //超过原文件大小则拓展，小于则截取，由此知道文件大小，单位字节
	//成功返回0，不是文件大小
	if(len == -1){
		perror("ftruncate");
		exit(1);
	}
	char *p = NULL;                                              //NULL          3种  SHARED,PRIVATE
	p = mmap(NULL, 4, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);//地址,映射区大小(字节),权限,是否反映到磁盘,fd,偏移量(字节)
#if 0
	新建一个文件，大小为零，参2填0，会出错。新文件必须拓展。     malloc不会
	p++后，munmap不能成功，不再是首地址了，  所以要拿一个临时指针变量来操作，保证首地址保存起来。
	偏移量1000(文件足够大)：出错，偏移量必须是4K(4096)的整数倍，MMU负责映射它是以4k为单位
	文件小于映射区大小，不行   文件大于映射区大小，可以
	文件描述符先关闭，没影响，文件描述符只是个句柄   而连接已经建立，以地址方式操作，和描述符没关系。
	必须检测mmap的返回值，太容易出错了。
	

	只读时，就不要用SHARED了

	open文件时只读，映射区读、写权限。会出错(权限不足)
	open文件时只读，映射区读、写权限、不反映到磁盘。会出错(总线错误)，硬件异常
	只写打开文件，只写映射区，出错(权限不足)
	权限：	
	映射区权限 <= 文件权限
	映射区创建过程中，隐含一个读操作，必须给个读权限
	只读SHARED,中途写了，会出现段错误，gdb直接run会停在出现段错误的地方
#endif
	if(p == MAP_FAILED){  //失败返回MAP_FAILED
		perror("mmap");
		exit(1);
	}
	//操作
	strcpy(p, "abc\n"); //写数据
	close(fd);
	int un = munmap(p, 4);//释放映射内存， 首地址、映射区大小
	if(un == -1){
		perror("munmap");
		exit(1);
	}

	return 0;
}
