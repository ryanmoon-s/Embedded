//-----------文件锁，只能用于非血缘关系的进程间----------
//有血缘关系的进程间和线程间都共享了文件描述符，所有对文件属性的修改都共享了。
#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>

void set_err(char*str){
	perror(str);
	exit(1);
}

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("a/.out filename\n");
		exit(1);
	}
	int fd = open(argv[1], O_RDWR);//这个不重要。反正后面都直接锁文件。没锁的进程可以打开文件，不能read write
	if(fd < 0){
		set_err("open");
	}
	struct flock f;
//	f.l_type = F_RDLCK;
	f.l_type = F_WRLCK;
	f.l_whence = SEEK_SET;
	f.l_start = 0;
	f.l_len = 0;
	//加锁
	fcntl(fd, F_SETLKW, &f); //f要取地址，加锁解锁都是F_SETLKW，加还是解取决于flock的属性W阻塞，不加不阻塞
	printf("get lock\n");
	sleep(10);  //模拟文件操作
	f.l_type = F_UNLCK;
	//解锁
	fcntl(fd, F_SETLKW, &f); 
	printf("unlock\n");
	close(fd);
	return 0;
}
