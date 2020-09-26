/*------------------------------------------epoll----------------------------------------------*/
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
#include<sys/epoll.h>

#define SER_IP "127.0.0.1" 
#define SER_PORT 6666
#define MAXFD 10

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
	//epoll
	int cfd, readfd;
	int n;
	struct sockaddr_in cli_addr;
	socklen_t cli_addr_len = sizeof(cli_addr);
	char bbuf[BUFSIZ];
	char buf[BUFSIZ];
	int se_ret;
	struct epoll_event evts[MAXFD];
	struct epoll_event levt;  //add lfd
	levt.events = EPOLLIN;
	levt.data.fd = lfd;
	int epfd = epoll_create(MAXFD);
	if(epfd == -1)
		set_err("epoll_create"); //wrap.c 里面的
	int ctl = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &levt);
	if(ctl == -1)
		set_err("epoll_ctl");
	while(1){
		se_ret = epoll_wait(epfd, evts, MAXFD, -1);
		if(se_ret == -1)
			set_err("epoll error");
		for(int t = 0; t < se_ret; t++){
			//不是读事件
			if(!evts[t].events & EPOLLIN)
				continue;
			//是读事件
			if(evts[t].data.fd == lfd){//accept action
				cfd = Accept(lfd, (struct sockaddr*)&cli_addr, &cli_addr_len);
				//print client info
				printf("Client IP:%s, Client PORT:%d\n", inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, bbuf, sizeof(bbuf)), 
						ntohs(cli_addr.sin_port));
				//save cfd
				levt.events = EPOLLIN;
				levt.data.fd = cfd;
				ctl = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &levt);
				if(ctl == -1)
					set_err("epoll_ctl");
			}else{//read action
				readfd = evts[t].data.fd;
				n = Read(readfd, buf, sizeof(buf)); 
				if((n == 0) || (n < 0 && errno == ECONNRESET)){ //正常或意外关闭
					printf("disconnect!\n");
					epoll_ctl(epfd, EPOLL_CTL_DEL, readfd, NULL);
					Close(readfd);
				}else{
					printf("receive: %s\n", buf);//received data/n 
					for(int i = 0; i < n; i++){
						buf[i] = toupper(buf[i]);  //to upper by bytes
					}
					Write(readfd, buf, n);
					bzero(buf, sizeof(buf));
				}
			}
		}
	}
	Close(lfd);
	return 0;
}
