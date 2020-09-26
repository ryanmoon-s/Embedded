多路IO转接：//把内核请来当帮手监听，监听到了给我反馈，我再去处理。   类似signal();
	1.让我连接
	2.让我写数据
	3.让我读数据
//都是可以不阻塞马上完成，如读数据时对方已经写了才叫我来读，立即读到数据


//可以跨平台，，，，poll不行(linux特有)
select--------------------------------------------------------------------------------------------------------------------------
	1.select能监听的文件描述符个数受限于FD_SETSIZE,一般为1024，//单纯改变进程打开的文件描述符个数不能改变,除非重新编译内核
		select监听文件个数
	2.解决1024以下客户端使用select是很合适的，但如果链接客户端过多，select采用的是轮询模型，会大大降低服务器效率
		不应在select上投入更多精力
	--
#include<sys/select.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>
int select(int nfds, fd_set *readfds, fd_set *writefds,  //fd_set 本质是位图，一个Int数
		fd_set *exceptfds, struct timeval *timeout);
nfds:监听的文件描述符集里最大文件描述符+1，因为此参数会告诉内核检测前多少个文件描述符的状态
-readfds:监控有读数据到达文件描述符的集合，传入传出参数//只是监听这个事件，对方不一定可读
-writefds:监控有写数据到达文件描述符的集合，传入传出参数 //传入的是我要监听的，传出的是满足条件的，用FD_ISSET判断哪些在里面
-exceptfds:监控异常发生文件描述符的集合，如带外数据到达异常，传入传出参数//成功了才传出.select返回后，全部重置为传出状态
timeout:定时阻塞时间，3种情况：
	1.NULL，永远等下去
	2.设置timeval，等待固定时间
	3.设置timeval里时间均为0，检查描述之际后立即返回，轮询
struct timeval {
	long    tv_sec;         /* seconds */
	long    tv_usec;        /* microseconds */
};

返回值：
	1.成功：所监听的所有(监听集合)中满足条件的总数 
		r:1 2 w:1 2 3 4 e:1 2 3-----9个
		成功了几个就返回几个
	2.失败：

如何判断谁满足了什么事件： //fd_set是个位图，一个Int数
	void FD_ZERO(fd_set *set);//将set清空 0
	void FD_SET(int fd, fd_set *set);//将fd设置在set集合中
	void FD_CLR(int fd, fd_set *set);  //将fd从set中清除
	int  FD_ISSET(int fd, fd_set *set); //判断fd是否在集合中

fd_set readfds;
FD_ZERO(&readfds);
FD_SET(fd1, &readfds);
FD_SET(fd2, &readfds);
select();----->总数
for(){
	int ret = FD_ISSET(fd1, &readfds); //----1 满足
}//类似于sig_t   用sigismmber打印哪些未决信号集

缺点：
	1.文件描述符上限1024
	2.如果监听3 1023两个文件，只能for(0--->1023)，除非我保存下来这两个值，每次在这两个里面去找(轮询)
	3.readfd 传入传出，当一传出，原来我设置的就没了，所以要保存下来

建立连接时监听lfd:
	读事件



poll-------------------------------------------------------------------------------------------------------------------------
int poll(struct pollfd *fds, nfds_t nfds, int timeout);          //select升级版：找一个'poll'帮忙监听
struct pollfd {
	int   fd;         /* file descriptor */
	short events;     /* requested events */  POLLIN 读   POLLOUT 写  POLLERR 出错
	short revents;    /* returned events */  位图//select提供了isset函数，这个没有，就revent & POLLIN 看是否为真
};
//fds:结构体数组的首地址
//nfds:数组中监听的元素个数-------
//timeout   毫秒级等待
//  -1 永久等待
//   0 立即返回，不阻塞进程
//   >0 等待指定毫秒数，如果系统时间精度不够毫秒，向上取值
相较于select优点：
	1.突破1024  //select只允许用户最多监听打开1024个，操作系统实际可以打开N多个。
	2.监听返回集合 分离了
	3.不用计算maxfd + 1
	4.搜索范围小(自己的数组) //缺点：但是1000个，只有3个满足，还是得遍历1000个。  epoll:告诉我哪些满足
成功返回revents不为0的个数，和select差不多//一个revents可能有多个事件，所以不能用--return 来判断

cat /proc/sys/fs/file-max  查看操作系统最多能打开的文件描述符上限//受硬件限制
ulimit -a 查看是可以打开1024个文件，但是可以修改：sudo vi /etc/security/limits.conf
	在文件尾部写入以下配置，soft软限制，hard硬限制：//小于上面的硬件限制
*	soft nofile 65536
*	hard nofile 1000000

fds[0].fd = lfd;
fds[0].events = POLLIN|POLLOUT|POLLERR
fds[0].revents//设置监听时，这个没用，直接不设置(或者0)，操作系统返回时会把它设为POLLIN ...

--断开连接时把events和revents清零，revents有值可能会进入后面的read永久阻塞(对方并未发数据)--

// if(fds[0].revents & POLLIN){}   //revents=0  ->  假，  revents=1  ->真


//linux下   不可跨平台(综合select poll)
epoll-------------------------------------------------------------------------------------------------------------------------
//能告诉我哪些满足了，不是轮询(selec/tpoll)。优势更明显       如果几乎全部监听，也差不多
#include<sys/epoll.h>
1.创建一个epoll句柄(windows下面的名词，linux用句柄，对用户屏蔽了实现细节)，size告诉内核监听的文件描述符个数，跟内存大小有关
	int epoll_create(int size);  //size 建议监听数目，创成10后面也可以监听50
	//返回一个文件描述符(指向内核一棵二叉树的root:平衡二叉树(左子树和右子树高度不超过1))--采用平衡二叉树的红黑树
2.控制某个epoll监控的文件描述符上的事件：注册、修改、删除
	int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
	//epfd上面的返回值
	//op: EPOLL_CTL_ADD添加   EPOLL_CTL_MOD修改   EPOLL_CTL_DEL删除  结点
	//fd 操作的文件描述符
	struct epoll_event {
		uint32_t     events;      /* Epoll events */  EPOLLIN EPOLLOUT EPOLLERR
		epoll_data_t data;        /* User data variable */
	};
	typedef union epoll_data {
		void        *ptr;
		int          fd;  //告诉我哪个fd满足了，联合体，传了这个就不传其它的成员了。(共用内存空间，按最大的分配)
		uint32_t     u32;
	    uint64_t     u64;
	} epoll_data_t;
3.等待所监控的文件描述符上有事件产生，类似于select()调用  开始阻塞等待
	int epoll_wait(int epfd, struct epoll_event *events,int maxevents, int timeout);
	//events 用于存内核得到事件的集合，上面是结构体地址，这个是数组(传出参数)
	//maxevents 告诉内核这个events有多大，不能大于epoll_create时的size
	//timeout 超时时间：和poll一样 -1阻塞 0立即返回 >0...
	返回：成功返回有多少文件描述符就绪，时间到时返回0，出错返回-1

//void *ptr 泛型指针什么都可以传，把下面这个传进去，有参数、有需要调的函数
	struct fdopt{
		int fd;
		void *argv;
		void (*func)(int fd, void *argv); //回调函数
	}

---------------epoll边沿触发epoll ET   水平触发epll LT //减小epoll_wait调用次数
发了1000字节来
epoll_wait();
Readn 读了500字节
还剩下500字节读不读？  边沿触发-不读  水平触发-读
//---------select poll epoll------能读任何文件描述符，不限于socket-------------
//管道也可以，父子进程通信  它们是助手
evt.events = EPOLLIN;  //默认水平触发
evt.events = EPOLLIN | EPOLLET;  //边沿触发 EPOLL ET 
//对面发来的包前多少个字节是描述，如果后面的不是需要的数据，就边沿触发，不要了。如果需要再epoll_ctl(epfd, EPOLL_CTL_MOD, ...)
//再定时清空缓冲区


---------------epoll非阻塞IO-------------------------------------
如果Readn 500  对方只发了200，服务器就阻塞在Readn了(永久阻塞) //Readn里面封装了while，直到读到多少为止
	解决办法：
	1. fcntl 修改socktefd属性 //sock用这个
	2. 重新open，添加不阻塞属性//对socketfd无效，不是Open来的

int flag = fcntl(cfd, F_GETFL);
flag |= O_NOBLOCK;
fcntl(cfd, F_SETFL, flag);

//最优模型：边沿触发 + read非阻塞
在这种情况下设成边沿触发，把读数据加在while(read() > 0)里面，这样和水平触发效果相同，但是只调用epoll_wait一次，并且非阻塞读，
	不会阻塞。(如果不设为非阻塞，read()>0会永久阻塞，一直读直到对方关闭或者错误)
	//非阻塞时，如果没数据了，会立即返回 -1 errno = EAGAIN  (sock 0还是对方关闭连接)
	

--------------epoll 反应堆模型---------------------------------
libevent库 --跨平台 精炼(使用了epoll的非阻塞IO模型)
//epoll反应堆模型，把libevent里面的主要内容抽出来了
1.epoll--监听--cfd--可读--epoll返回--read--cfd从树上摘下--设置监听cfd写事件、操作--小写转大写--等待epoll_wait返回--回写--
	cfd从树上摘下--设置监听读事件、操作--epoll继续监听 
	//读到数据后，多了步重新设置成它的写事件，满足时才写回去，写完后再设回读事件。(应付滑动窗口，对方可能缓冲区满了，服务器会阻塞)
2.原：evt[i].event & EPOLLIN, evt[i].data.fd == cfd
  现：用data里面的*ptr  struct{
							int fd;
							void *arg;
							void (*func)(void *arg);
						}





