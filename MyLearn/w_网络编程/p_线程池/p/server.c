/*------------------------------------------高并发服务器线程版---------------------------------------------------*/
#include<stdio.h>
#include"wrap.h"  //my err pak
#include<signal.h>
#include<pthread.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<strings.h>//bezero   strings
#include<string.h>
#include<unistd.h>
#include<ctype.h> //toupper
#include<sys/socket.h> //socket
#include<arpa/inet.h>  //transfer sockaddr_in

#define SER_IP "127.0.0.1" 
#define SER_PORT 6666

typedef struct data_t{
	struct sockaddr_in cli;
	int cfd;
}data_t;

void* pth_ser(void* arg){
	data_t* data = (data_t*)arg;
	//print client info
	char bbuf[BUFSIZ];
	printf("Client IP:%s, Client PORT:%d\n", inet_ntop(AF_INET, &data->cli.sin_addr.s_addr, bbuf, sizeof(bbuf)), 
			ntohs(data->cli.sin_port));
	//read and write
	char buf[BUFSIZ];
	int n;
	while(1){
		n = Read(data->cfd, buf, sizeof(buf)); 
		if(n == 0){
			printf("disconnect!\n"); 
			break;
		}else{
			printf("receive: %s\n", buf);//received data/n 
		}
		for(int i = 0; i < n; i++){
			buf[i] = toupper(buf[i]);  //to upper by bytes
		}
		Write(data->cfd, buf, n);
		bzero(buf, sizeof(buf)); //like memset
	}
	close(data->cfd);
	pthread_exit(NULL);
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
	//accept
	pthread_t tid;
	int cfd;
	data_t data;
	struct sockaddr_in cli_addr;
	socklen_t cli_addr_len = sizeof(cli_addr);
	while(1){
		cfd = Accept(lfd, (struct sockaddr*)&cli_addr, &cli_addr_len);
		data.cli = cli_addr;
		data.cfd = cfd;
		tid = pthread_create(&tid, NULL, pth_ser, (void*)&data);
		if(tid != 0)
			printf("pthrad_create error: %s\n", strerror(tid));
		pthread_detach(tid);
	}
}
