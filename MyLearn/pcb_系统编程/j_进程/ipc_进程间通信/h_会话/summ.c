创建会话：
	
注意事项：
	1.调用进程不能是进程组组长，该进程变成新会话首进程(session header)
	2.该进程成为一个新进程组的组长进程(会话里有一个进程级，它是组长兼会长)
	3.需有root权限(ubuntu不需要)
	4.新会话丢弃原有的控制终端，该会话没有控制终端
	5.该调用进程是组长进程，则出错返回
	6.建立会话时，先调用fork，父进程终止，子进程调用setsid
	7.原默认会长是bash，bash里面就把运行的c程序自立门户了，新程序的组长就是自己

getsid函数：
	//获取进程所有的会话id
	pid_t getsid(pid_t pid); 成功返回调用进程的会话id 失败-1 errno
setsid函数：
	pid_t setsid(void); 成功返回设置好的... 失败-1 errno
