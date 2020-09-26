#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/stat.h>


int main(int argc, char* argv[]){
	if(argc < 3){
		perror("/a.out filename modeNumber");
		exit(1);
	}
	long int mod = strtol(argv[2], NULL, 8);
	int ret = chmod(argv[1], mod);
	if(ret == -1){
		perror("chomd");
		exit(1);
	}else{
		printf("succesful!\n");
	}
}
