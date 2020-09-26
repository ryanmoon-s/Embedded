#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>

//typedef void (*sighandler) (int);
void catc(int x){
	printf("----------%d has been catched\n", x);
}  //不用typedef，估计也应该写成这种参数和返回值


int main(){
	struct sigaction act;
	act.sa_handler = catc; //屏蔽函数
	sigemptyset(&act.sa_mask); //清空sa_mask
	sigaddset(&act.sa_mask, SIGQUIT);  //加入屏蔽信号
	act.sa_flags  = 0;  //自动屏蔽本信号

	int re = sigaction(SIGINT, &act, NULL);//注册信号捕捉
	if(re == -1){
		perror("sigaction");
		exit(1);
	}
while(1);

}
