#include<stdio.h>
#include<string.h> //strlen
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
	//my socket
	int lfd = Socket(AF_INET, SOCK_STREAM, 0);
	//server addr
	struct sockaddr_in ser_addr;
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(SER_PORT);
	inet_pton(AF_INET, SER_IP, &ser_addr.sin_addr.s_addr);
	//connect
	Connect(lfd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
	//read and write
	char buf[BUFSIZ];
	int n;
	while(1){
		fgets(buf, sizeof(buf), stdin); //gets is unsafe  myworld\n  ->   myworld\n\0  send myworld\n
		Write(lfd, buf, strlen(buf)); //because of the \0 fgets filled , stelen can be used
		n = Read(lfd, buf, sizeof(buf));//read from lfd to buf
		Write(STDOUT_FILENO, buf, n);//write to screen
		bzero(buf, sizeof(buf)); //like memset
	}
	close(lfd);
	return 0;
}
