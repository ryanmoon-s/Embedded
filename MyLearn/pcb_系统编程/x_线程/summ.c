//linux和windows的线程很不一样。以前unix是没有线程的，后来用的人多，没办法强加的(根据进程)
//linux进程已经很完美了，线程和进程差别不大
//尽量不要把线程和信号搅一起，给自己找麻烦，信号和线程的关系是硬套进去的，不好实现。复杂

线程：//LWP：light weight process 轻量级进程，本质仍是进程
	1.进程有独立地址空间，有pcb
	2.线程没有独立地址空间，共用空间，也有pcb  //pthread_create 就是新建一个pcb，从进程变成了线程
	3.线程是最小执行单位，cpu分配时间轮片的时候，给每一个线程分配时间轮片
	4.进程是最小分配资源单位，所有线程共享一块资源(进程分得的4G资源)

内核实现：
	1.fork pthread_create -> 调用clone() 
	2.从内核里看进程和线程是一样的，都有各自不同的pcb，但是pcb中指向内存资源的三级页表是相同的
		pcb里某个指针->页目录，里面某个指针->页表，里面某个指针->物理页面，里面是内存单元了  MMU映射过程
		新线程中那个指针也复制过来了(Pcb是独立的)，指向相同的页目录...共享资源
	3.进程可以蜕变成线程
	4.线程可以看做寄存器和栈的集合
	main ebp 栈基指针  
		 	...						
		 	...(栈帧)				
	 	 esp 栈顶指针			
调用func ebp //ebp和esp只有一个，调用func后，它们下称了，同时下面保存了上面一块的位置值 
			...
			...
		esp
		//内核里面也有栈，保存寄存器的值，cpu时间片切换时...
	5.查看LWP号：ps -Lf pid 查看指定进程lwp号(cpu分配时间轮片的依据)，//不是线程id(进程内部区分线程)
		打开firfox浏览器，多打开几个标签，一搜，很多线程

线程共享资源：
	1.文件描述符表
	2.每种信号的处理方式
	3.当前工作目录
	4.用户id和组id
	5.内存地址空间(.text/.data/.bss/heap/共享库) 0-3G 栈以外，特殊errno变量非共享
//全局变量
线程非共享：
	1.线程id
	2.处理器现场和栈指针(内核栈) ebp esp
	3.独立的栈空间(用户空间栈)
	4.errno变量
	5.信号屏蔽字
	6.调度优先级

线程优点：
	1.提高程序并发性 2.开销小 3.数据通信、共享数据方便
线程缺点：
	1.库函数，不稳定(进程是系统调用) 2.调试、编写困难、gdb不支持(printf调试) 3.对信号支持不好
	优点相对突出，缺点不是硬伤。linux下由于实现方法导致进程、线程区别不大

线程控制原语
	pthread_self函数
	//获取线程id
	pthread_t pthread_self(void);  成功id，失败:无！
	线程id:pthread_t类型，本质:在linux下为无符号整数，unsigned long -> %lu，其它系统可能是结构体实现
	！！不用使用全局变量pthread_t tid 在子线程中通过pthread_create传出参数来获取线程Id....而应该使用pthread_self
	编译、链接时，使用 -pthread 参数  这是第三方库

	pthread_create
	//创建新线程
	int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void*(*start_routine)(void*), void* arg);
	成功0 失败:错误号，  --linux下线程特点:失败返回错误号
	参1:传出参数，保存系统为我们分配好的线程id
	参2:通常传NULL，表示使用线程默认属性，若想修改具体属性也可修改该参数
	参3:函数指针，指向线程主函数(线程体)，该函数运行结束，则线程结束
	参4:线程主函数执行期间所使用的参数

线程退出：
	退出单个线程    //pthread_exit((void*)1);  接收它的指针本身值就是1，直接(int)re输出。如果是传出指针，re->
	void pthread_exit(void *retval); retval表示线程退出状态，通常传NULL   //exit是退出进程,不要在线程中用
	可以在主控线程中用pthread_exit 退出主控线程，其它线程就不会退出，仍然在运行，就不用sleep 再return。 
	//如果在主控线程中用exit return 就是进程退出，其它线程也没了
	//线程中return NULL;返回到调用者 与 pthread_exit相似，如果在调用的其它函数中调用这两个就不一样了。 

回收线程：
	阻塞等待线程退出，获取线程退出状态
	int pthread_join(pthread_t thread, void **retval); 成功0 失败:错误号
	参一：线程id，不是指针。 //pthread_create是指针
	参二：retval，存储线程结束状态
	//void*传出，void**接收  参数传出值必须多一个*才能传出，和返回值接收不一样
	//一般 int * ret     (void**)&ret    *ret输出值 
	可以将i传入线程，线程通过i来判断是哪个线程
	不像进程只能父进程回收，线程谁都可以回收

线程分离：
	进程中没有类似的，实现线程分离
	int pthread_detach(pthread_t thread);  成功 0， 失败:错误号
	1.线程分离状态：指定该状态，线程主动与主控线程断开关系。线程结束后，其退出状态不能由其它线程获取，而自己自动释放
		不用再回收了，只是状态分离，地址空间不变。网络、多线程服务器常用//自动回收pcb
	2.进程中若有该机制，将不会产生僵尸进程，僵尸进程的产生是因为进程死后大部分资源被释放，一点残留资源仍在系统中，导致
		内核认为该进程仍然存在//pcb残留
	3.也可用pthread_create函数参二(线程属性)，来设置线程分离
	4.分离了再join回收，直接报错，不会阻塞等待,int x = pthread_join ，x为errno 22
	//fprintf(stderr, "...%\ns", strerr(x));
	有可能create还没来得及返回，设置成分离的线程就结束了，它的线程号和系统资源可能被移交给其它线程使用
	那么create返回的就是一个错误的线程号，这种概率很低，但也可能发生，要采取一定的措施：如
		//在线程里调用pthread_cond_timedwait函数，让这个线程等一会，有时间让create返回，这是在多线程里常用的方法。
		//注意不要使用sleep等，这是让进程休眠，并不能解决线程同步的问题·

杀死线程：
	杀死(取消)线程
	int pthread_cancel(pthread_t thread);  成功0 失败:错误号
	1.不是实时的，有一定的延时，需要等待线程到达某个取消点(检查点)
		//类似于游戏存档，必须到达特定的场所才能存储进度，杀死线程也必须到达取消点
	2.取消点：是线程检查是否被取消，并按请求进行动作的一个位置，通常是一些系统调用:creat open pause close read write..
			执行命令man 7 pthreads 可以查看具备这些取消点的系统调用列表
	3.可粗略认为一个系统调用(进入内核)为一个取消点，如果线程中没有取消点，可以通过调用pthread_testcancel()自行设置一
		个取消点。
	4.被取消的的线程：退出值定义在linux的pthread库中。常数PTHRED_CANCELED的值是-1，可在头文件pthread.h中查到它的定义
		#define PTHREAD_CANCELED ((void*)-1) 
		//因此当我们对一个取消的线程使用join回收时，得到返回值-1  (void**)k 
	杀死的线程可以回收，返回值-1

pthread_equal函数
	比较两个线程id是否相等
	int pthread_equal(pthread_t t1, pthread_t t2);
	目前Id都是整数，可以直接==判断，以后linux可能会被改成结构体

线程属性//attribute 属性
	一般默认属性能满足，有些时候也可以自己设
	应该先初始化线程属性，再创create创建
	初始化线程属性：
		int pthread_attr_init(pthread_attr_t* attr); 成功0，失败:错误号//attr传出参数
	销毁线程属性所占用的资源
		int pthread_attr_destroy(pthread_attr_t* attr); 成功0，失败:错误号
		-----------------------------------------------------------------------
	typedef strut{  
		int detachstate; //线程分离状态  1
		int stackaddr_set; //线程栈设置 (默认大小-> ulimit -a,显示的进程栈大小，理论上线程栈均分这个空间)
		                       //大小不够，用从堆申请来给他用
		size_t guardsize; //线程栈末尾警戒缓冲区大小
		...
	}pthread_attr_t;
	//上面列了主要用的三个，
	线程栈末尾警戒区：
		两个线程栈空间中间有缝隙，称为警戒区：防止线程溢出
	设置线程分离：
		设置线程属性 分离or非分离
			int pthread_attr_setdetachstate(pthread_attr_t* attr, int detachstate);
		获取线程属性 分享or非分离
			int pthread_attr_getdetachstate(pthread_attr_t* attr, int *detachstate);
			参数： attr:已初始化的线程属性
			detachstate: PTHREAD_CREATE_DETACHED (分离线程)
		  				 PTHREAD_CREATE_JOINABLE (非分离线程)
	设置栈空间：
		int pthread_attr_setstack(pthread_attr_t* attr, void* stackaddr, size_t stacksize);
		int pthread_attr_getstack(pthread_attr_t* attr, void** stackaddr, size_t* stacksize);
	修改栈大小：
		int pthread_attr_setstacksize(pthread_attr_t* attr, size_t stacksize);
		int pthread_attr_getstacksize(pthread_attr_t* attr, size_t* stacksize);

NPTL
	1.查看当前pthread库版本 getconf GNU_LIBPTHREAD_VERSION
		库不一样，运行可能异常
	2.NPTL实现机制(POSIX)，Native POSIX Thread Library
	3.使用线程库时gcc 指定 -pthread

线程使用注意事项：
	避免僵尸线程：
		pthread_join 回收
		pthread_detach 分离
		pthread_create 指定分离属性
		//被Join线程可能在函数返回前就释放完所有的内存资源，所以不应当返回被回收的线程栈中的值 
		//malloc 和mmap 申请的内存可被其它线程回收(共享的)
		//应该避免在多线程模型中调用fork，除非马上exec，一个线程fork后，子进程中只有调用fork的线程存在，其它线程
			均pthread_exit了
		//信号的复杂语义很难和多线程共存，应当避免多线程引入信号机制

usleep 微秒级睡眠 	


