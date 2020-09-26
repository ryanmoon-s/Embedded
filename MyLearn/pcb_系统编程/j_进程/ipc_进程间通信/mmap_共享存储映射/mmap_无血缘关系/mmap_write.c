#include<stdio.h>
#include<stdlib.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
//总结：和单纯的文件操作相似，但是建立映射区后可以两个进程并行运行，一个写，一个读。并且也就提供了一个stu的大小的内存
//记得打开文件和下面建立时，都始终要有读权限。只读时PRIVATE。新文件一定要ftruncate拓展内存
//文件是支持两个进程一起打开的，也可以用文件读写来并行通信。。。。下面说明了为什么
//strace   追踪可执行文件做了些什么，进行文件操作时，发现底层一直在用mmap，对文件进行操作修改。

//可多读端，多写端。必须要用同一文件

void set_err(char* str){
	perror(str);
	exit(1);
}

typedef struct{
	int id;
	char name[20];
	char sex;
}stu;

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("/a.out filename\n");
	}
	//open file
	int fd = open(argv[1], O_RDWR);//就算是写，也要给读权限，初始化会读一次
	if(fd == -1){
		set_err("openfile");
	}
	ftruncate(fd, sizeof(stu));
	//set
	stu *p = NULL; 
	p = mmap(NULL, sizeof(stu), PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
	if(p == MAP_FAILED){  //失败返回MAP_FAILED
		set_err("mmap");
	}
	//操作
	stu st1 = {1, "Alice", '0'};
	while(1){
		memcpy(p, &st1, sizeof(stu));
		st1.id++;
		sleep(1);
	}
	//释放
	close(fd);
	int un = munmap(p, sizeof(stu));//释放映射内存， 首地址、映射区大小
	if(un == -1){
		set_err("munmap");
	}

	return 0;
}
