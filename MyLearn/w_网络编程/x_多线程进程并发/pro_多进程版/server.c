/*------------------------------------------高并发服务器进程版---------------------------------------------------*/
#include<stdio.h>
#include"wrap.h"  //my err pak
#include<signal.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<strings.h>//bezero   strings
#include<unistd.h>
#include<ctype.h> //toupper
#include<sys/socket.h> //socket
#include<arpa/inet.h>  //transfer sockaddr_in

#define SER_IP "127.0.0.1" 
#define SER_PORT 6666

void recycle(int arg){   //p1:0同组子进程，p2进程状态，p3是否阻塞
	while(waitpid(0, NULL, WNOHANG) > 0);  //多个信号来，只发一次信号。如果有子进程在排队：>0成功一个继续回收
//	while(1); 测试，在信号处理函数里面死循环，主线程也不干活了，说明这是一种中断机制，不能在信号处理函数里面呆太久
	//如果阻塞，要么阻塞等不到回收，要么回收一个就走了。不能把所有死了的都回收。
	//之前都是在父进程里面，事做完了，阻塞回收。这种是信号来了回收了再回去做事，只能不阻塞。这种方式
	return;										//直到=0，没有子进程可回收，退出，不固定死亡个数就这样写。
}

int main(){
	//socket
	int lfd = Socket(AF_INET, SOCK_STREAM, 0);
	//bind
	struct sockaddr_in ser_addr;
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(SER_PORT);
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	Bind(lfd, (struct sockaddr*)&ser_addr, sizeof(ser_addr)); 
	//listen
	Listen(lfd, 128); //default
	//catch sig
	struct sigaction sig;
	sig.sa_handler = recycle;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = 0;  //0：处理期间屏蔽本信号，SA_NODEFER不屏蔽(重入)		SA_RESTART 信号处理完后重启(慢速系统调用)
	sigaction(SIGCHLD, &sig, NULL);          //那得是可重入函数才行
	//accept
	pid_t pid;
	int cfd;
	struct sockaddr_in cli_addr;
	socklen_t cli_addr_len = sizeof(cli_addr);
	while(1){
		cfd = Accept(lfd, (struct sockaddr*)&cli_addr, &cli_addr_len);
		if(cfd > 0){
			if(-1 == (pid = fork())){
				perror("fork");
			}
			if(pid == 0)
				break;//child exit
		}
		close(cfd);
	}
//-------------------child----------------------------------------------------------------------------------------
	if(pid > 0)
		exit(1);
	close(lfd);
	//print client info
	char bbuf[BUFSIZ];
	printf("Client IP:%s, Client PORT:%d\n", inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, bbuf, sizeof(bbuf)), 
			ntohs(cli_addr.sin_port));
	//read and write
	char buf[BUFSIZ];
	int n;
	while(1){
		n = Read(cfd, buf, sizeof(buf)); 
		if(n == 0){   //socket pipe  0：对端关闭
			printf("disconnect!\n"); 
			break;
		}else{
			printf("receive: %s\n", buf);//received data/n 
		}
		for(int i = 0; i < n; i++){
			buf[i] = toupper(buf[i]);  //to upper by bytes
		}
		Write(cfd, buf, n);
		bzero(buf, sizeof(buf)); //like memset
	}
	close(cfd);
	return 0;
}
