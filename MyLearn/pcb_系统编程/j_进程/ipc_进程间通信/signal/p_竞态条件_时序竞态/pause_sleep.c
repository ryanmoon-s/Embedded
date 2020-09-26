#include<stdio.h>
#include<signal.h>
#include<unistd.h>
typedef void (*sighandler_t)(int);
//利用pause等待信号，和alarm计时，来实现自己的sleep

void mysleep(int sec){
	alarm(sec);
	int re = pause();
	if(re == -1){
		perror("pause catch");//fitch返回后，再设置errno
	}

	alarm(0);//如果alarm没有到pause就被其它信号终止了，所以需要清零闹钟
	//如果是sigaction就在这里sigaction(SIGALRM, &oldact, NULL);回复状态
}

void fitch(int x){
	;
}

int main(){
	//注册信号捕捉
	sighandler_t x  = signal(SIGALRM, fitch);
	//sleep
	while(1){
		mysleep(3);
		printf("---sleep finish---\n");
	}
}
