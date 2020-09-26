#include<stdio.h>
#include<stdlib.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>

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
	int fd = open(argv[1], O_RDONLY);
	if(fd == -1){
		set_err("openfile");
	}

	//set
	stu *p = NULL; 
	p = mmap(NULL, sizeof(stu), PROT_READ, MAP_PRIVATE, fd, 0);
	if(p == MAP_FAILED){  //失败返回MAP_FAILED
		set_err("mmap");
	}
	//操作
	while(1){
		printf("id = %d, name = %s, sex = %c\n", p->id, p->name, p->sex);
		sleep(2);
	}
	//释放
	close(fd);
	int un = munmap(p, sizeof(stu));//释放映射内存， 首地址、映射区大小
	if(un == -1){
		set_err("munmap");
	}

	return 0;
}
