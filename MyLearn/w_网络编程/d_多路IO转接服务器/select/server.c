/*------------------------------------------select----------------------------------------------*/
#include<stdio.h>
#include"wrap.h"  //my err pak
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<strings.h>//bezero   strings
#include<string.h>
#include<unistd.h>
#include<ctype.h> //toupper
#include<sys/socket.h> //socket
#include<arpa/inet.h>  //transfer sockaddr_in
#include<sys/time.h>  //select
#include<sys/select.h>  //select

#define SER_IP "127.0.0.1" 
#define SER_PORT 6666

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
	Listen(lfd, 10); //default
	//select
	int cfd, readfd;
	int n;
	struct sockaddr_in cli_addr;
	socklen_t cli_addr_len = sizeof(cli_addr);
	char bbuf[BUFSIZ];
	char buf[BUFSIZ];
	fd_set in, bak;  //read; 
	int se_ret;
	int maxfd = lfd;  //最大文件描述符，select用，刚开始最大的就是它(只有它)，后面加的肯定比它大
	int top = -1;  //数组最后一个值的下标，遍历数组用
	int allfd[FD_SETSIZE];  //FD_SETSIZE  --->1024
	for(int i = 0; i < FD_SETSIZE; i++)
		allfd[i] = -1;
	FD_ZERO(&bak);
	FD_SET(lfd, &bak);
	while(1){
		in = bak; //多个来连接，只有循环一次处理一个(lfd只能保存一个)
		se_ret = select(maxfd + 1, &in, NULL, NULL, NULL); //参一表示最大的<文件描述符大小>+1
		//monitoring accept action
		if(FD_ISSET(lfd, &in) == 1){
			cfd = Accept(lfd, (struct sockaddr*)&cli_addr, &cli_addr_len);
			//print client info
			printf("Client IP:%s, Client PORT:%d\n", inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, bbuf, sizeof(bbuf)), 
					ntohs(cli_addr.sin_port));
			for(int i = 0; i < FD_SETSIZE; i++){
				//save cfd
				if(allfd[i] == -1){
					allfd[i] = cfd;
					FD_SET(cfd, &bak);
					if(maxfd < cfd)
						maxfd = cfd;
					if(top < i)
						top = i;
					break;
				}
			}
			//overflow
			if(maxfd == FD_SETSIZE - 1){
				printf("too many client connect!");
				exit(1);
			}
			//only connect 避免后面不必要的判断
			if(--se_ret == 0)
				continue;
		}
		//monitoring read action
		for(int i = 0; i <= top; i++){
			readfd = allfd[i];
			if(FD_ISSET(readfd, &in)){
				n = Read(readfd, buf, sizeof(buf)); 
				if((n == 0) || (n < 0 && errno == ECONNRESET)){  //收到RESET 对方意外关闭连接
					printf("disconnect!\n");
					FD_CLR(readfd, &bak);
					close(readfd);
					allfd[i] = -1;
					continue;
				}else{
					printf("receive: %s\n", buf);//received data/n 
					for(int i = 0; i < n; i++){
						buf[i] = toupper(buf[i]);  //to upper by bytes
					}
					Write(readfd, buf, n);
					bzero(buf, sizeof(buf));
					if(--se_ret == 0)
						break; //处理完成，避免后面不必要的for循环
				}
			}
		}
		//monitoring error
		if(se_ret == -1){
			perror("select error");
			exit(1);
		}
	}
	close(lfd);
	return 0;
}
