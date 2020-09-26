#include<stdio.h>
#include<signal.h>
#include<unistd.h>
//打印未决信号集


int main(){
	sigset_t myset, oldset, ped;

	sigemptyset(&myset);//清空
	sigaddset(&myset, SIGQUIT);//加入
	sigaddset(&myset, SIGINT);//加入
	sigaddset(&myset, SIGTSTP);//加入
   //现在只能另开一个终端kill了

	sigaddset(&myset, SIGKILL);//  不起作用，另一个终端仍然能杀死它

	sigprocmask(SIG_BLOCK, &myset, &oldset);//加入到阻塞信号集

	//	raise(SIGQUIT);//发送信号       普通状态下未决信号集始终为0，发送了一个信号，它变为1，阻塞了后，它不能翻转回来
	//  在终端按Ctrl + ' \'.....

	while(1){
		sigpending(&ped);//输出未决信号集到Ped
		for(int i = 1; i < 32; i++){
			if(sigismember(&ped, i) == 1){
				putchar('1');
			}else{
				putchar('0');
			}
		}
		printf("\n");
		sleep(1);
	}
	return 0;
}
