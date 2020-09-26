中断系统调用/信号中断系统调用

系统调用分为两类：慢速系统调用 其它系统调用
	1.慢速系统调：用可能会造成永久阻塞：pause wait waitpid read(管道 网络 设备(键盘)) 
    	阻塞期间收到信号，不再回来继续执行(早期)，//系统调用被信号中断了->信号中断系统调用
	2.其它系统调用：getpid getppid fork...//一下就执行完了

如read:buff不满，可能是在读管道、读到末尾、文件关闭、 被信号打断->返回-1 设置errno
1.

通过判断errno是不是EINTER(被信号打断)，来决定是否重新启动该调用。//read想回来，pause不想回来

2.
捕捉，有的信号默认动作是忽略，有的是关闭进程，都不能继续进行	
通过修改sa_flags设置被信号中断后是否重启 SA_INTERRUPT不重启 SA_RESTART重启//前提是捕捉该信号
//这样只能针对某个信号，来了后自动重启。想要被所有的信号中断(关闭进程外)后重启，用1.判断errno，angin:  goto again;
设为SA_NODEFER 执行期间不阻塞该信号，即重入(sa_mask中没有屏蔽)，0 屏蔽该信号
