#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>  //stat
#include <stdio.h>  //perror
#include <stdlib.h>  //exit

//main函数参数，argc取得的参数个数，arg[0]此文件名，arg[1]传入的字符串
int main(int argc, char* argv[]){
	if(argc < 2){
		printf("./a.out filename\n");
		exit(1);
	}
	struct stat st;
	//stat将前一个参数的文件类型，加载到后一个参数中，传出。st中就有了该文件的所有信息，可以在它上面读取相应的文件信息
	//stat ，称为追踪函数(穿透)，能追踪到软链接的文件，读取具体大小。  lstat读软链接的大小
	int ret = lstat(argv[1], &st);
	if(ret == -1){
		perror("stat");
		exit(1);
	}
	//获取文件大小
	int size = (int)st.st_size;
	printf("file size = %d\n", size);
}
