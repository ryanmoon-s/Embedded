内核传送 内核处理  kill -l     man 7 signal   编程过程尽量用宏名

产生信号：
	1.按键产生：Ctrl+c Ctrl+z Ctrl+\
	2.系统调用：Kill raise abort
	3.软件条件产生：定时器 alarm
	4.硬件异常产生：非法访问内存(段错误，指针啥的) 除0(浮点数例外) 内存对齐出错(总线错误，mmap申请空间过大)
	5.命令产生：kill命令

递达：递送并到达(瞬时)
未决：信号被阻塞/屏蔽了(阻塞态)--->集合：阻塞信号集(信号屏蔽字)--->未决信号集

	pcb结构体中有两个重要成员：未决信号集、阻塞信号集(信号屏蔽字)   set   不重复，无序  一个存放00001101的数组
	a[i]，i是编号1 2 3 ...32  a[i]内容为信号 一来个信号，0->1 送达马上 1->0  (未决信号集)
														 0->1 未决信号集不能翻转了(阻塞信号集)
信号的处理：
	1.执行默认动作  -line30-
	2.忽略(丢弃)  不是不处理，处理就是选择丢弃它，未决信号集->0
	3.捕捉(调用户处理函数)，逮住来正理这个信号

信号四要素：
	1.编号 2.名称 3.事件 4.默认处理动作

kill -l 查看信号
	1-31号是常规信号，2ctrl c 3键盘输入退出 7总线错误 8浮点运算例外 9杀死进程信号 10 11 12 13 14 15 17 19 20
	34-64是实时信号，驱动开发用
	man 7 signal 查看帮助文档  3个值linux取中间。另外两个是其它类unix平台的

默认动作： -line18-
	1.Term：终止进程
	2.Ign：忽略信号  -父进程接收到子进程死亡时
	3.Core：终止进程，生成Core文件(核心已转储)。(查验进程死亡原因，用于gdb高度)  -段错误，-g直接run
	4.Stop：停止(暂停)进程
	5.Cont：继续运行进程

特殊信号：
	9号SIGKILL  杀死进程信号 
	19号SIGSTOP  停止进程
	不允许忽略和捕捉，只能执行默认动作。   防止有病毒文件把所有信号设为忽略或者捕捉，就不能杀死该进程

终端按键产生信号：
	Ctrl+c ->2  SIGINT(终止/中断)   INT Interrupt 终止终端启动的进程
	Ctrl+\ ->3  SIGQUIT(退出)                     终止终端启动的进程，并转储核心
	Ctrl+z ->20 SIGTSTP(暂停/停止)	T  Terminal 终端   暂停交互进程的运行

硬件异常产生信号：
	除0操作      8  SIGFPE(浮点数除外)
	非法访问内存 11 SIGSEGV(段错误)
	总线错误     7  SIGBUS

kill函数(发送命令，不是杀死进程) 终端里面的命令也是：
	成功返回，失败返回-1(id非法、信号非法、越权)，设置error
	kill(pid_t pid, int sig);   sig建议使用宏名，不要用数字，有些不同系统数字不同
	pid>0  发送给指定信号
	pid=0  发送给调用Kill函数同一进程组的所有进程         杀同一进程组的全部
	pid<0  取|pid|发给对应进程组                          杀指定的一组全部
	pid=-1 发给(有权限发送的)系统中的所有进程

raise和abort函数：
	raise给当前进程发指定信号
		int raise(int sig);  0成功   失败非0
	abort给自己发  6 SIGABRT  终止，生成核心文件
		int abort();  无返回值
	信号不要乱发
	
软件条件产生信号：
	alarm函数：
		unsigned int alarm(unsigned int seconds);  //秒级
		设置定时器，指定seconds后，内核给当前进程发送14 SIGALRM信号，进程收到信号默认终止
		每个进程有且仅有唯一一个定时器，Fork后，子进程有自己的定时器
		alarm(0);取消定时器，返回旧闹钟剩下秒数 
		alarm(3):设个新信号3S，取消之前的闹钟、返回之前alarm剩下秒数
		自然定时法，与进程状态无关。(就绪、运行、挂起[阻塞 暂停]、终止、僵尸)都要发
			alarm(1);
			for(int i = 0; ; i++)
				printf("%d ",i);   //测试电脑一秒能数多少数 ,如果重定向到文件，相差非常大，说明io非常耗时，优化主要从IO入手
				//	time ./a.out 计算进程消耗时间  real实际 user 用户空间 sys内核中消耗时间
				//  后两者加起来应该等于前者，但是不相等，因为有等待时间，大部分在等待硬件什么IO，还有cpu...
			
	setitimer函数：不如alarm重要
		也是定时器，精度微秒。周期性定时。      成功返回0 失败返回-1 
		int setitimer(int which,const  struct itimerval *new value, struct itimerval *old value);//前者传入 后者传出
		which:指定定时方式：
			1.自然定时 ITIMER_REAL ->  14 SIGALRM    计算自然时间  一般用这个
			2.虚拟空间计时(用户空间) ITIMER_VIRTUAL ->26 SIGVTALRM 只计算进程占用cpu的时间
			3.运行计时器(内核+用户) ITIMER_PROF -> 27 SIGPROF 计算占用cpu用执行系统调用的时间
		itimerval 里面又含有 两个结构体成员  it_interval{tv_sec;tv_usec}以后所有的间隔时间 it_value{} 初始定时时间
				//写一个捕捉去打印，末尾加上死循环interval设为3，value设为1，第一次3s打印，以后都是隔1s打印
		itimerval x;  直接x.it_value.tv_sec=1  定时1s  其它调为0  sec 秒 usec微秒

信号捕捉：
	1.signal 函数
	sighandler_t signal(int signum, sighandler_t handler);   //捕捉信号交给函数指针指这个函数处理
	//注册 ..信号捕捉处理函数，只是注册，   内核捕捉，调用这个参二的函数去...
	//把参二的函数写在main外面，没有调用 ---->回调，信号捕捉是一个回调函数
	typedef void(*sighandler_t)(int);   //参二为一个函数指针  返回值为void 参数为int的函数传进去就行了。 
	有可能要自己定义 这样 typedef 后，sighandler_t 就是一个这种函数指针的类型名  相当于int x中的int
	//sighandler_t x = signal();         if(x == SIG_ERR)    perror

	2.sigaction 函数
	int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);//参三可NULL
	注册信号 参二保存老的处理动作
	struct 中 最后一个sa_restorer废弃了，sa_sigaction暂时不用
	    sa_handler, 处理函数     
		sa_mask，捕捉信号期间的信号屏蔽集，防止执行函数中途又有信号来了，只在函数执行期间mask起效，原屏蔽字不起作用
		sa_flags, 函数执行期间，又来了这个信号，又没加入屏蔽集，    通常设为默认值0 ->函数执行期间自动屏蔽本信号
		阻塞的常规信号不排队，只记录一次，执行完函数后就会处理
	正常运行->来信号->进入内核处理信号->有捕捉函数->进入用户空间执行函数->sys_sigreturn回到内核->回到中断的地方继续执行

信号集操作函数：
	信号屏蔽字mask可以影响未决信号集，这里自定义set来改变mask，达到屏蔽信号的目的
	sigset_t set;  //typedef unsigned long sigset_t
	int sigemptyset(sigset_t *set)            //清空信号集                     成功0 失败-1
	int sigfillset(sigset_t *set)              //置1信号集                     成功0 失败-1
	int sigaddset(sigset_t *set, int signum)   //将某个信号加入信号集          成功0 失败-1
	int sigdelset(sigset_t *set, int signum)   //将某个信号从信号集中删除      成功0 失败-1  重复操作，一样
	int sigismember(sigset_t *set, int signum)   //判断某个信号是否在信号集中    1在 0不在
	sigset_t本质是位图，但是不应该直接位操作，应该使用函数，保证跨系统有效
	1 2 3 4 5是对应好的，在里面就是1，不在里面就是0
	用自定义的set  ->  阻塞信号集  ->未决信号集
	阻塞3号信号，sigset_t myset,oldset;  sigemptyset(&myset);  sigaddset(&myset, 3);
	->  sigprocmask(SIG_BLOCK, &myset, &old);

	sigprocmask 函数    只能写入到当前进程的阻塞信号集中
	用来屏蔽信号、解除屏蔽   本质是读取或修改进程的信号屏蔽字(pcb中)
	int sigprocmask(int how, const sigset_t* set, sigset_t* oldset);  成功0失败-1设置error // 参三可NULL
	set 传入参数 set中哪个位置为1，就表示当前进程屏蔽哪个信号
	oldset 传出参数，保存旧的信号屏蔽集
	how :  (假设当前进程的信号屏蔽字为mask)
		1.SIG_BLOCK: 当how设为此值，set表示需要屏蔽的信号 相当于mask = mask | set
		2.SIG_UNBLOCK:...           set表示需要解除屏蔽的信号。相当于mask = mask & ~set
	  	//都是操作表中为1的，阻塞它，解除它
		3.SIG_SETMASK:...           set表示用于替代原始屏蔽及新屏蔽集，相当于mask = set 谨慎使用
		调用sigprocmask解除了对当前若干个信号的阻塞，则在sigprocmask返回之前至少将其中一个信号递达

	sigpending函数     读当前进程的未决信号集
	int sigpending(sigset_t* set);  //set付出参数，成功0 失败-1 设置error
	用sigismember 用查看打印哪个信号在不在里面






