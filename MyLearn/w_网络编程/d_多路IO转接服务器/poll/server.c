/*------------------------------------------poll----------------------------------------------*/
#include<stdio.h>
#include"wrap.h"  //my err pak
#include<errno.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<strings.h>//bezero   strings
#include<string.h>
#include<unistd.h>
#include<ctype.h> //toupper
#include<sys/socket.h> //socket
#include<arpa/inet.h>  //transfer sockaddr_in
#include<poll.h>

#define SER_IP "127.0.0.1" 
#define SER_PORT 6666
#define MAXFD 1024

int main(){
	//socket
	int lfd = Socket(AF_INET, SOCK_STREAM, 0);
	//bind
	struct sockaddr_in ser_addr;
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(SER_PORT);
	inet_pton(AF_INET, SER_IP, &ser_addr.sin_addr.s_addr);
	Bind(lfd, (struct sockaddr*)&ser_addr, sizeof(ser_addr)); 
	//listen
	Listen(lfd, 128); //default
	//poll
	int cfd, readfd;
	int n;
	struct sockaddr_in cli_addr;
	socklen_t cli_addr_len = sizeof(cli_addr);
	char bbuf[BUFSIZ];
	char buf[BUFSIZ];
	int se_ret;
	int fdnum = 1;  //最大文件描述符，select用，刚开始最大的就是它(只有它)，后面加的肯定比它大
	int top = -1;  //数组最后一个值的下标，遍历数组用
	struct pollfd allfd[MAXFD];
	for(int i = 0; i < MAXFD; i++)
		allfd[i].fd = -1;
	allfd[0].fd = lfd;
	allfd[0].events = POLLIN;
	while(1){
		se_ret = poll(allfd, fdnum, -1);
		//monitoring accept action
		if(allfd[0].revents & POLLIN){
			cfd = Accept(lfd, (struct sockaddr*)&cli_addr, &cli_addr_len);
			//print client info
			printf("Client IP:%s, Client PORT:%d\n", inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, bbuf, sizeof(bbuf)), 
					ntohs(cli_addr.sin_port));
			for(int i = 1; i < MAXFD; i++){
				//save cfd
				if(allfd[i].fd == -1){
					allfd[i].fd = cfd;
					allfd[i].events = POLLIN;
					fdnum++;
					if(top < i)
						top = i;
					break;
				}
			}
			//overflow
			if(fdnum == MAXFD){
				printf("too many client connect!");
				exit(1);
			}
		}
		//monitoring read action
		for(int i = 1; i <= top; i++){
			if((readfd = allfd[i].fd) == -1)  //poll里面必要，allfd[i]的fd是等于-1，但是allfd[i]的revents可能还有值，导致read不到
				continue;                            //清零了就应该不存在
			if(allfd[i].revents & POLLIN){
				n = Read(readfd, buf, sizeof(buf)); 
				if(n < 0){
					//接收到RESET标识，对方意外关闭连接
					if(errno == ECONNRESET){
						printf("disconnect!\n");  //不改变top的值也行，每加入一个fd大于它，它会更新
						allfd[i].fd = -1;
						//事件清零，不然还要监听它，内核对events不为0的结构体按照个数进行监听，可能导致应该监听的-没	
						allfd[i].events = 0; 
						//如果不重置，后面这个位置有文件描述符了，它就会直接进来读，然而没数据，阻塞
						allfd[i].revents = 0; 
						fdnum--;
						close(readfd);
						continue;
					}
				}else if(n == 0){
					//对方主动关闭连接
					printf("disconnect!\n");
					allfd[i].fd = -1;
					allfd[i].events = 0; 
					allfd[i].revents = 0; 
					fdnum--;
					close(readfd);
					continue;
				}else{
					printf("receive: %s\n", buf);//received data/n 
					for(int i = 0; i < n; i++){
						buf[i] = toupper(buf[i]);  //to upper by bytes
					}
					Write(readfd, buf, n);
					bzero(buf, sizeof(buf));
					//if(--se_ret == 0)
					//	break;  一个revents可能有读有写
				}
			}
		}
		//monitoring error
		if(se_ret == -1){
			perror("poll error");
			exit(1);
		}
	}
	close(lfd);
	return 0;
}
