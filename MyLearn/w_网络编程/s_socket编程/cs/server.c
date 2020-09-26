#include<stdio.h>
#include"wrap.h"  //my err pak
#include<stdlib.h>
#include<sys/types.h>
#include<strings.h>//bezero   strings
#include<unistd.h>
#include<ctype.h> //toupper
#include<sys/socket.h> //socket
#include<arpa/inet.h>  //transfer sockaddr_in

#define SER_IP "127.0.0.1" 
#define SER_PORT 6666

int main(){
	//socket
	int lfd = Socket(AF_INET, SOCK_STREAM, 0);
	//setopt//port reuse
	int opt = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));  //非常复杂 unp里面有，day03.08
	//bind
	struct sockaddr_in ser_addr;
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(SER_PORT);
	inet_pton(AF_INET, SER_IP, &ser_addr.sin_addr.s_addr);
	Bind(lfd, (struct sockaddr*)&ser_addr, sizeof(ser_addr)); 
	//listen
	Listen(lfd, 128); //default
	//accept
	struct sockaddr_in cli_addr;
	socklen_t cli_addr_len = sizeof(cli_addr);
	int cfd = Accept(lfd, (struct sockaddr*)&cli_addr, &cli_addr_len);//the second argument output for client info
	//print client info
	char bbuf[BUFSIZ];
	printf("Client IP:%s, Client PORT:%d\n", inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, bbuf, sizeof(bbuf)), 
			ntohs(cli_addr.sin_port));
	//read and write
	char buf[BUFSIZ];
	int n;
	while(1){
		n = Read(cfd, buf, sizeof(buf)); //in socket, return 0 reference to disconnect.in pipe and file ref to end.
		if(n == 0){
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
	close(lfd);
	return 0;
}
