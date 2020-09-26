#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>                     //open
#include<stdio.h>    //printf  perror
#include<unistd.h>   //exit
#include<stdlib.h>   //exit

int main(){
	int fd;
	//打开一个已经存在的文件，返回值是文件描述符，文件描述符表数组中的下标，失败返回-1
	fd = open("f.txt", O_RDWR);
	if(fd == -1){
		perror("open file");
		exit(1);
	}

	//创建新文件  creat和excl同时用，判断文件是否存在，     O_TRUNC将文件内容删除
	fd = open("new", O_RDWR | O_CREAT | O_EXCL, 0777);//0代表八进制，可不加   
	/*
	本地有一个掩码，用umask查看，文件实际权限：给的权限 与 本地掩码的取反 按位& 
	002 -> 000 000 010 -> 111 111 101
	               777 -> 111 111 111 
   						  111 111 101
						   7   7   5
						           rx
	umask 022   临时修改umask
	*/
	if(fd == -1){
		perror("open file");
		exit(1);
	}
	printf("fd=%d\n", fd);
	//关闭文件
	if(fd == -1){
		perror("open file");
		exit(1);
	}
	int ret = close(fd);
	printf("ret = %d\n", ret); //ret=0  关闭成功
}
