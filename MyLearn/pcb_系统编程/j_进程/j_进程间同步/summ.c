进程间同步

信号量：(1-N)
	sem_init(&sem, PTHREAD_PROCESS_SHARED);//进程共享同样应该放在映射区，进程都能访问到
	//PTHREAD_PROCESS_PRIVATE 线程间用


互斥量：
	进程间也可以使用互斥锁同步，但应该在pthread_mutex_init之前修改属性变量为进程共享
	//并把锁放在内存映射区，不同的进程都能访问这把锁，才能实现加锁解锁
主要应用函数：//设置mutex的属性参数，方法和Mutex的初始化差不多
	pthread_mutexattr_t mattr// 类型，用于定义mutex锁的属性
	pthread_mutexattr_init //初始化一个mutex属性对象
		int pthread_mutexattr_init(pthread_mutexattr_t* attr);
	pthread_mutexattr_destroy //销毁mutex属性对象(非销毁锁)
		int pthread_mutexattr_destroy(pthread_mutexattr_t* attr);
	pthread_mutexattr_setpshared//修改mutex属性
		int pthread_mutexattr_destroy(pthread_mutexattr_t* attr, int pshared);
		参2：pthread取值：
			线程锁：PTHREAD_PROCESS_PRIVATE(mutex默认属性即为线程锁，进程间私有)
			进程锁：PTHREAD_PROCESS_SHARED

以前：pthread_mutex_init(&mutex, NULL);//默认为线程锁
现在：
	pthread_mutexattr_t attr; //创建一个attr
	pthread_mutexattr_init(&attr); //初始化一个attr
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED); //设为进程锁
	pthread_mutex_init(&mutex, &attr);
	//现在为进程锁了

文件锁：
fcntl函数：//可以锁文件部分内容
	借助它来实现锁机制，操作文件的进程没有获得锁时，可以打开，但无法执行read write操作
	fcntl函数作用：获取、设置文件访问属性，直接设置文件的属性，让别的进程不能访问。父子进程不算，它们共享了文件描述符
	int fcntl(int fd, int cmd, .../* arg */);
	//fcntl(fd, F_SETLKW, &flock);  flock为一个结构体
cmd: F_GETLK 
 	 F_SETLK  //设置锁 非阻塞 
	 F_SETLKW //设置锁 阻塞 wait
	struct flock{
		... //允许文件部分加锁
		short l_type;  //方式：F_RDLCK F_WRLCK F_UNLCK 读写解
		short l_wence;  //从哪开始计算起始位置：SEEK_SET SEEK_CUR SEEK_END
		off_t l_start; //开始的偏移量
		off_t l_len; //锁的字节数 0表示锁到文件结尾，以append追加时也会锁，无限向后
		pid_t l_pid; //获得持有该锁进程的Id，只能F_GETLK用
		...
	}
//多线程不能用这个，多线程共享文件描述符，设置的这些属性，都共享了。所以你加锁成功，相当于我加锁成功
//同理父子进程也不能用，也共享文件描述符(内存映射区，->读写锁)
//多线程可以用自己的读写锁，也是这个功能

fcntl的其它cmd:
	F_GETFL:  //get flag
	F_SETFL: O_WRONLY ...O_RDONLY...O_APPEND...
	//文件里面就是一串二进制位001101，(位图)，用位或和与来操作
	int flag = fcntl(fd, F_GETFL, 0);
	flag |= O_BLOCK  //或上阻塞属性，就有了阻塞了  flag &= !O_BLOCK 就没有这个属性了
	fcntl(fd, F_SETFL, 0);
