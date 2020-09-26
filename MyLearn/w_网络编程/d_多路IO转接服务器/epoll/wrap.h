//头文件申明
//主文件包含这个文件，wrap.c也包含这个文件
//直接从man复制，把首字母改成大写
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/types.h>
#include<strings.h>//bezero   strings
#include<unistd.h>
#include<ctype.h> //toupper
#include<sys/socket.h> //socket
#include<arpa/inet.h>  //transfer sockaddr_in


void set_err(const char*str);
int Socket(int domain, int type, int protocol);
int Bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen);
int Accept(int s, struct sockaddr *addr, socklen_t *addrlen);
int Connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
int Listen(int s, int backlog);
ssize_t Read(int fd, void *buf, size_t count);
ssize_t Write(int fd, void *buf, size_t count);
int Close(int fd);
ssize_t Readn(int fd, void *buf, size_t count);
ssize_t Writen(int fd, void *buf, size_t count);
