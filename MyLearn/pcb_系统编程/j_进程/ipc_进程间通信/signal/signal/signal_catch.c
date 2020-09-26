#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
typedef void (*sighandler_t) (int);
//这样之后，以后sighandler_t就代表这个函数指针的类型名   sighander_t x    x就是这种类型的函数指针

void catchs(int x){
	printf("------catched-----\n");
}

int main(){
	
	sighandler_t si = signal(SIGINT, catchs); //注册信号捕捉函数
	if(si == SIG_ERR){
		perror("catch");
		exit(1);
	}
	while(1);
		
}
