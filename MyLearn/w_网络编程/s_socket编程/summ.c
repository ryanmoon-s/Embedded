套接字---------------------------------------------
	//socket(有插座的意思)一边是插头，一边是插座     // 一定是成对出现，要么没有，要么成对
	linux内核借助缓冲区将socket形成一个伪文件，统一接口，屏蔽对用户的差别，用户就可以像操作文件一样，操作scoket。
	类似fifo。但是fifo用于本地进程间通信，socket应用于网络进程间数据传递//内核实现非常复杂，不必深入研究
	--在TCP/协议中，IP地址+TCP/UDP端口号唯一标识网络通讯中的一个进程。IP+端口号就是一个socket，想要建立连接的两个
	--进程，各自有一个socket，这两个socket组成的socket pair就唯一标识一个连接，因此用socket表示网络连接的一对一的关系
//IP：在网络中唯一标识一台主机
//Port:在主机中唯一标识一个进程
//IP+Port：在网络中唯一标识一个进程(这个进程就叫soket)
//linux占内存的文件：普通文件 目录 软链接 //不占的：字符设备 块设备 管道 套接字
//socket：创建后一个fd------缓冲区1读    双向全双工可入可出
//					  ------缓冲区2写
//	fd1  发送端---------->接收端  fd2
//		 接收端<----------发送端
ip地址32位，端口号16位



网络字节序-----------------------------------------
	//大端：低地址--高位(高地址--低位)//ibm开始这样存储，网络当时主要是他们在开发
	//小端：低地址--低位(高--高) 一些计算机内部//Inter早年开始
	TCP/IP协议规定，网络数据流应该采用大端字节序，即低地址高字节，和某些计算机内部相反，为保证可移植性，
		同样的c程序在大端和小端计算机上都能运行，所以提供了以下函数作网络字节序和主机字节序的转换。
	#include<arpa/inet.h>
	uint32_t htonl(uint32_t hostlong);   //host to net long htonl 32位，转ip   (INADDR_ANY)---绑定本地可用ip
	uint16_t htons(uint16_t hostshort);  //h->n port
	uint32_t ntohl(uint32_t netlong);	 //n->h ip
	uint16_t ntohs(uint16_t netshort);   //n->h port
	如果主机是小端，这些函数将参数做相应的转换后返回，如果主机是大端，这些函数不做转换，将参数原封不动的返回



linux中ip不用上面的函数，用：--------------------------
	#include<arpa/inet.h>    //阿帕 inet
	int inet_pton(int af, const char* src, void* dst); //ip to net 点分十进制的ip，直接从192.168.(字符串)->网络字节序
	const char* inet_ntop(int af, const void* src, char* dst, socklen_t size);//直接从网络字节序->192.168.(字符串)
pton参一：                                                  
		AF_INET  //ipv4,现在主要用这个
		AF_INET6 //ipv6
	参二：源ip，字符串型(点分十进制)192.168..
	参三：传出参数，数值型
ntop参二：源ip，数值型
	参三：传出参数，字符串型，和返回值一样，//是个缓冲区
	参四：参三传出缓冲区的大小


sockaddr数据结构-------------------------------------
	很多网络编程早于ipv4协议，那时候用的sockaddr，后来为了兼容------>
	早期用的struct sockaddr 后来改了数据结构，名字改了struct sockaddr_in，以前的数据结构废弃了，而编译器只认以前的名
	所以在用的时候要：struct sockaddr_in addr;       //历史遗留问题
					  bind(  , (sockaddr*)&addr);
					  accept();
					  connect();
//man 7 ip查看
truct sockaddr_in {
	sa_family_t sin_family; /* 地址族: AF_INET */	  addr.sin_family = AF_INET/AF_INET6
	u_int16_t sin_port; /* 按网络字节次序的端口 */    addr.sin_port = htons/ntohs
	struct in_addr sin_addr; /* internet地址 */       addr.sin_addr.s_addr = htonl/ntohl/inet_pton/inet_ntop
};
/* Internet地址. */
struct in_addr {
	u_int32_t s_addr; /* 按网络字节次序的地址 */
};




网络套接字函数---------------------------------------
#include<sys/types.h>
#include<sys/socket.h>
--int socket(int domain, int type, int protocol);	 //protocol 协议
//新建套接字文件描述符
domain:
	AF_INET //ipv4   TCP或UDP
	AF_INET //ipv6
	AF_UNIX //本地协议，本地套接字通信用
type:
	SOCK_STREAM //按顺序、可靠、数据完整，默认使用TCP协议
	SOCK_DGRAM //无连接、固定长度，不可靠，默认使用UDP协议 dgram数据报，报式协议
	SOCK_SEQPACKET				 //双线路、可靠、发送固定长度的数据包，必须把包完整接收才能读取
	SOCK_RAW					//提供单一的网络访问，使用ICMP公共协议(ping\tracerance)
	SOCK_RDM					//很少使用，大部分操作系统未实现，提供给数据链路层，不保证数据包的顺序
protocol:
	传0表示使用默认协议
返回值：
	成功：返回指向新创建socket的文件描述符，失败-1设置nerrno

--int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
//将套接字绑定至ip+port
sockfd:
	socket文件描述符
addr:
	构造出IP地址+端口号
addrlen:
	sizeof(addr)
返回值：	
	成功 0，失败-1设置errno

--int listen(int sockfd, int backlog);//指定监听数值，同时允许多少客户端与我建立连接，不是我支持多少
sockfd:
	socket文件描述付
backlog:
	排除建立3次握手队列和刚刚建立3次握手队列的链接数和
返回值：
	成功 0 。失败-1 设置errno
//查看系统默认backlog   cat /proc/sys/net/ipv4/tcp_max_syn_backlog

--int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
//接收一个连接请求，服务器调用
sockfd:
	socket文件描述符
addr:
	传出参数，返回链接客户端地址信息，含IP地址和端口号 //服务器调，不需要初始化addr
addrlen:
	传入传出参数(值-结果)，传入sizeof(addr)大小，函数返回时返回真正接收到的地址结构体大小
返回值：
	成功返回一个_新的_socket文件描述符，用于和客户端通信，失败返回-1，设置errno

--int connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
//，客户端调用，连接服务器
sockfd:
	socket文件描述符 //int fd = socket();  直接传入来接收服务器的socket文件描述符
addr:
	传入参数，指定服务器地址信息，含ip + port
addrlen:
	传入参数，传入sizeof(addr)大小
返回值：
	成功 0 。失败-1 设置errno
//和bind形式一致，bind是自己的addr，connect是对方的addr



socket模型创建流程图----------------------------------
服务器 socket()                socket() TCP客户端
	   bind()                  //客户端没有bind会自动分配一个随机的ip和port-->隐式绑定
	   listen()				   //客户端bind，可以依赖隐匿绑定，也可以绑定一个自己的
阻塞   accept()<-------------->connect()
       read()<-----------------write()//请求数据
	  //处理请求
	   write()---------------->read()//回应数据
	   read()<-----------------close()
	   close()
//服务器必须bind，不然如果随便分配，客户端不知道连哪里
//先ctrl c 服务器，后ctrl c 客户端，服务器端口并未释放。所以在建立端口时要检查是否被占用，perror
//过一会后会被释放   netstat -apn | grep 6666  查看


测试服务器--------------------------------------------
命令：nc 目标ip     服务器端口号
	  nc 127.0.0.1  6666
	  //net connect


TCP状态分析-------------------------------------------
netstat -apn | grep 6666 查看端口运行状态
//网络编程的第三天的03、04里面查看图
2MSL:主动断开连接的一方-->FIN----> (对方ACK FIN ) --->ACK   //处于：TIME_WAIT
然后等待一段时间，确保对方收到自己的ACK。(如果对方未收到ACK，会回发FIN，我再ACK)
	//这段时间过了----->CLOSE
RFC 793 中规定MSL为2分钟，实际应用常用的是 30秒 1分钟 2分钟 ...(linux一般  半分钟).
//(RFC是一系列以编号排定的文件，收集了有关因特网的相关资讯，以及UNIX和因特网社群的软件文件)
	1.如果服务器主动断开某个客户端的连接，服务器端口就会进入TIME_WIT状态，在此期间，这个端口的服务就不能启动了 
	2.这时候客户端来连接就连接不上了。客户端主动断开连接，客户端也不能用原端口连接，但是客户端port是随机的，无所谓
//解决办法：在建立socket时属性设SO_REUSEDDR,这个端口就可以启动了。
//但socket pair依然存在，即在服务器time_wait时又启动了服务，如果客户端的端口号和Ip还是原来的，就不能连接。
time_wait时间多久？--->2MSL，不要答1min //不会用上，但是要知道这个理论
RST: A---->SYN  //三次握手时
     B--->SYN  ACK    A由于其它原因(除非网断了)不能连接了，操作系统发一个RST到服务器。服务器就准备重置了，不等它了


TCP半关闭----------------------------------------------、
	1.close可以实现半关闭(在写方，看作是全关闭，不能读进来了)
		dup2 ----两个文件描述符指向套接字，这时close就不行了
	2.shutdown 
	
#include<sys/socket.h>
int shoutdown(int sockfd, int how);
how:	
	SHUT_RD(0);  关闭sockfd上的读，该socket不再接受任何数据，缓冲区的数据无声的丢掉
	SHUT_WR(1);  关闭sockfd上的写，该socket不再写入数据，进程不能对它进行写操作
	SHUT_RDWR(2); 关闭sockfd的读写。相当于调用shutdown再次，先rd 后 wr //一个sockfd有两个缓冲区
	//close是关闭某个文件描述符，shutdown是关闭所有的指向socket的连接


端口复用-------------------------------------------------书  apue(系统编程)   UNP(网络编程)
	在server的TCP连接没有完全断开时不允许重新监听是不合理的，就算ip不同时端口号仍相同，还是不能重启监听
		所以，使用setsockopt()设置sock描述符的选项SO_REUSEADDR为1，：允许创建端口号相同，但ip不同的多个sock描述符
//在server的socket()和bind之间插入：
		int opt = 1;
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));  //非常复杂 unp里面有，day03.08
		//sockfd, level, optname, optvalue, optsize



