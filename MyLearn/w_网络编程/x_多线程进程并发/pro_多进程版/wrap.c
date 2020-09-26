#include"wrap.h"

//一个容错模块
//减少主c文件代码量，加强逻辑，将系统函数diy上自己的功能。加强系统函数的功能，如慢速系统调用被信号中断的处理方式等。
//首字母大写仍然能shift+k查看
//返回值形参和原函数一样，从man复制过来。if里面记得删类型

void set_err(const char*str){
	perror(str);
	exit(1);
}

int Socket(int domain, int type, int protocol){
	int ret;
	if(-1 == (ret = socket(domain, type, protocol)))
		set_err("socket");
	return ret;
}

int Bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen){
	int ret;
	if(-1 == (ret = bind(sockfd, my_addr, addrlen)))
		set_err("bind");
	return ret;
}

int Listen(int s, int backlog){
	int ret;
	if(-1 == (ret = listen(s, backlog)))
		set_err("listen");
	return ret;
}

//慢速系统调用
int Accept(int s, struct sockaddr *addr, socklen_t *addrlen){
	int ret;
again:
	if(-1 == (ret = accept(s, addr, addrlen))){
		if((errno == ECONNABORTED) || (errno == EINTR))   //accept属于慢速系统调用，可能被信号中断
			goto again;		//当被异常中断和被信号打断时回到打断前的状态再次执行goto again
		else
			return -1;  //多进程时，父进程揽活，如果接收一个client失败，不希望exit，所以return -1
	}
	return ret;
}

int Connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen){
	int ret;
	if(-1 == (ret = connect(sockfd, addr, addrlen)))
		set_err("connect");
	return ret;
}

//慢速系统调用
ssize_t Read(int fd, void *buf, size_t count){
	ssize_t ret; //记得和函数返回值一致
again:
	if(-1 == (ret = read(fd, buf, count))){
		if(errno = EINTR) //读socket ，返回0：对方关闭连接。管道、文件是到末尾
			goto again; //errno = EAGAIN(EWOULDBLOCK) 以非阻塞的方式读的。返回了
		else
			set_err("read");
	}
	return ret;
}

//慢速系统调用
ssize_t Write(int fd, void *buf, size_t count){
	ssize_t ret;
again:
	if(-1 == (ret = write(fd, buf, count))){
		if(errno = EINTR)
			goto again;
		else
			set_err("write");
	}
	return ret;
}

int Close(int fd){
	int ret;
	if(-1 == (ret = close(fd))){
		set_err("close");
	}
	return ret;
}

//参三：应该读取的字节数，网络读取常用，OS没提供，需要自己封装，ssize_t有符号整数
//如果要的字节超过1500，read读到1500就返回了(对方一个包只发了1500),自己封装一个能读超过1500的函数
ssize_t Readn(int fd, void *buf, size_t count){
	size_t nleft; // >0还剩下多少没读
	ssize_t nread;  //实际读到的字节数
	char* ptr;  //用来挪buf的存指针

	nleft = count;
	ptr = buf;
	while(nleft > 0){ //还剩下有没法读的
		if(-1 == (nread = read(fd, ptr, nleft))){  //这一句是必须执行的，socket中只返回具体读到的字节数
			if(errno == EINTR)
				nread = 0;//被中断，未读入
			else
				set_err("read");//其它错误，直接退出
		} else if (nread == 0) //sock里面是返回0断开连接，所以不管是否读完一管，都返回具体读到的字节数
			break;
		nleft -= nread; //读完一管
		ptr += nread; //写指针后挪
	}
	return count - nleft; //具体读到的字节数，对方断开连接的话，就没读满
}

//写入多少数据，同上
ssize_t Writen(int fd, void *buf, size_t count){
	size_t nleft;
	ssize_t nwrite;
	char* ptr;

	nleft = count;
	ptr = buf;
	while(nleft > 0){
		if(-1 == (nwrite = Write(fd, ptr, nleft))){  //返回0则没写数据
			if(errno == EINTR )
				nwrite = 0;
			else
				set_err("read");
		}
		nleft = nwrite;
		ptr += nwrite;
	}
	return count; //不存在没写完，要么写完退出while，要么错误退出。
}


//Readline 读一行，如100字节
