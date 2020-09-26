#nclude<stdio.h>
#include<signal.h>
#include<unistd.h>
typedef void (*sighandler_t)(int);
//利用pause等待信号，和alarm计时，来实现自己的sleep
//优化后的mysleep，用sigsuspend替代pause


void mysleep(int sec){
	//屏蔽alarm信号
	sigset_t myset, oldset, mymask;
	sigemptyset(&myset);
	sigaddset(&myset, SIGALRM);
	sigprocmask(SIG_UNBLOCK, &myset, &oldset);
	//设定自己的mask，把旧mask删除信号屏蔽
	mymask = oldset;//oldset用于恢复屏蔽前的状态
	sigdelset(&mymask, SIGALRM);
	//执行
	alarm(sec);//信号是屏蔽的
	int re = sigsuspend(&oldset);//执行时信号被打开，保证能接收到
	if(re == -1){
		perror("pause catch");
	}
	//恢复屏蔽状态
	sigprocmask(SIG_SETMASK, &oldset, NULL);


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
