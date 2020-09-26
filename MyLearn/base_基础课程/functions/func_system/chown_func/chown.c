#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/stat.h>


int main(int argc, char* argv[]){
	if(argc < 2){
		perror("/a.out filename"); 
		exit(1);
	}
	//uid 和 gid 在/etc/passwd里面看，关于passwd的说明在man 5 passwd里面看
	//121 126
	int ret = chown(argv[1], 121, 126);
	if(ret == -1){
		perror("chown");
		exit(1);
	}else{
		printf("succesful!\n");
	}
}
