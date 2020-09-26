同步：同指协同、协助、互相配合。主旨在协调步调，按预定的先后次序运行


线程同步：
	//指一个线程发出某一功能调用时，在没有得到结果之前，该调用不返回。同时其它线程为保证数据一致性，不能调用该功能
	多个控制流共同操作一个共享资源时，都需要同步

锁不具有强制性：建议锁


互斥量/互斥锁(mutex)----------------------------------
	建议锁，不按规则来访问，仍会数据混乱//多把锁
	pthread_mutex_init//初始化
	pthread_mutex_destroy//销毁
	pthread_mutex_lock//锁，阻塞等待加锁
	pthread_mutex_trylock//尝试加锁，失败返回，不阻塞，应该周期使用
	pthread_mutex_unlock//解锁
	返回都是：成功0 失败错误号
	pthread_mutex_t 类型，本质上是一个结构体，为简化理解，使用时忽略实现细节，简单看成整数
	pthread_mutex_t mutex;  mutex变量只有两种取值：1、0  //初始化成功->1

pthread_mutex_init
	//初始化一个互斥量-->初值可看作1
	int pthread_mutex_init(pthread_mutex_t* restrict mutex, const pthread_mutexattr_t* restrict attr);
	参1：传出参数，调用时传&mutex
	restrict(限制)：只用于限制指针，告诉编译器，所有修改指针指向内容的操作，只能通过本指针完成，不能通过除了本
		指针以外的其它变量或指针修改//意思是Mutex这把锁不能赋值给别个，让别个能锁东西，不是说就把要访问的东西锁了
	参2：互斥量属性，传入参数，通常为NULL，选用默认属性(线程间共享)
		1.静态初始化：如果互斥锁mutex是静态分配的(定义在全局，或加了static修饰)，可以直接使用宏进行初始化。
			pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
		2.动态初始化；局部变量应该动态初始化 
			pthread_mutex_init(&mutex, NULL);

pthread_mitex_destroy
	int pthread_mutex_destroy(pthread_mutex_t* mutex);
	销毁一个互斥锁

pthread_mutex_lock//阻塞
	int pthread_mutex_lock(pthread_mutex_t* mutex);
	加锁，可理解为mutex-- / mutex-1

pthread_mutex_unlock
	int pthread_mutex_unlock(pthread_mutex_t* mutex);
	解锁，可理解为mutex++ / mutex+1

pthread_mutex_trylock
	int pthread_mutex_trylock(pthread_mutex_t* mutex);
	尝试加锁//不阻塞

死锁：
	1.线程试图对同一个互斥量加锁两次  //锁在’别人‘手上，我阻塞等着锁
	2.线程1拥有A锁，请求获得B锁。线程2拥有B锁，请求获得A锁
		//当不能获取所有的锁时，放弃已经占有的锁, 所有线程按相同顺序加锁
	3.震荡，哲学家吃饭问题


读写锁---------------------------------------
	//一把锁具三种状态：
	1.读锁
	2.写锁
	3.不锁
	写独占，读共享，
	写锁优先级高 //a正在读或写，b想写，c想读，并行阻塞。优先满足写的
					//a...b c 都想读 ，才能加成功
					//只要有写在等待，就算是在读，其它读也不能去读。
	适用场景：读的地方比写的地方多,互斥量就是不相容，相当于全写的读写锁。如果我的线程读的多，用这个效率高很多
主要应用函数：
	pthread_rwlock_init
	pthread_rwlock_destroy
	pthread_rwlock_rdlock
	pthread_rwlock_wrlock
	pthread_rwlock_tryrdlock
	pthread_rwlock_trywrlock
	pthread_rwlock_unlock//就一把锁，解了就是了
	都是成功0 失败错误号
	先定义一个pthread_rwlock_t 类型，定义一个读写锁变量
	
pthread_rwlock_init
	int pthread_rwlock_init(pthread_rwlock_t* restrict rwlock, const pthread_rwlock_t* restrict attr);
	参二:attr属性，通常传默认属性NULL
...都和上面mutex差不多
读写锁和mutex加锁和解锁方式差不多，(想要访问锁里面的东西，我主动等锁)只是，加锁的机制不一样



条件变量(不是锁！但它可以造成线程阻塞，通常与互斥锁一起使用，给多线程一个会合的场所)
	//相对于Mutex而言可以减少竞争，用mutex生产者与消费者之前有竞争，消费者与消费者之间也有竞争
	//有了条件变量后，可以减少生产者之间的竞争，当条件满足时再竞争，提高程序运行的效率
	//生产者是生产东西的，和消费者之间完全没必要竞争。用条件变量可以实现
主要函数：
	pthread_cond_init
	pthread_cond_destroy
	pthread_cond_wait    //难点
	pthread_cond_timedwait
	pthread_cond_signal  //去唤醒阻塞在条件变量上的至少一个线程
	pthread_cond_broadcast //广播的意思，唤醒所有
	都是成功0 失败错误号
	pthread_cond_t 类型，用于定义条件变量(这种都是结构体)
使用方法都和上面的mutex rwlock差不多
pthread_cond_wait
	int pthread_cond_wait(pthread_cond_t* restrict cond, pthread_mutex_t* restrict mutex);
	参二：一把初始化好并加了锁的互斥锁
	作用：
		1.阻塞等待条件变量cond(参1)满足
		2.释放已掌握的互斥锁(和1一起做)，相当于 pthread_mutex_unlock(&mutex);
		    //(1.2是一个原子操作)
		3.当被唤醒，pthread_cond_wait返回时，解除阻塞并重新申请获取互斥锁pthread_mutex_lock(&mutex);
			//阻塞等锁  //sigal broadcast唤醒
pthread_cond_timedwait
	//时间一到就返回了
	int pthread_cond_timedwait(pthread_cond_t* restrict cond, pthread_mutex_t* restrict mutex, const struct timespec
			* restrict abstime);//abslute 绝对的
	timespec在man sem_timedwait 里面有，tv_sec 秒 tv_nsec纳秒
	//它是从1970年1月1日00.00开始的，unix诞生元年，所以计算时：
		time_t cur = time(NULL); //1970至今的秒数
		struct timespec t;
		t.tv_sec = cur + 1;//计时1秒

//也可以多生产者，没意义，唯一的意义是做饼的时间长，一个做好了等待另一个通知吃货，然后吃了后，共享区空了出来，
//我放上去
生产者消费者模型1：   //单生产者，多消费者，减少了消费者之间的竞争，通知了才抢，更好调节生产者与消费者之前的配合
	线程->线程同步-> 生产者消费模型(必提)
	//用条件变量
	1.前提：师傅生产饼，吃货取饼
	2.条件变量：饼是否生产好(条件变量也只是个0 1模型，不用实例化)
	3.吃货pthread_cond_wait 等待条件满足 (这之前已经做了mutex的定义 init lock cond的定义 init)，并释放了锁
	4.师傅饼做好了，通过pthread_cond_signal或_broadcast通知吃货
	5.吃货被唤醒，解除阻塞申请锁，有锁了再去取饼


信号量/进化的互斥锁	(相当于把互斥量的1-N)
	//和信号没有任何关系，像java和javascript
	由于互斥锁粒度大，使用互斥锁时，虽然解决了共享资源的问题，但是多线程相当于变成了单线程，并发性下降
	//信号量，是折中的一种处理方式，既可同步，又不混乱，还提高并发性
主要应用函数：
	sem_init
	sem_destroy
	sem_wait//相当于加锁 信号量--
	sem_trywait
	sem_timedwait
	sem_post//相当于解锁 信号量++
	以上6个函数都是：成功返回0，失败-1，设置错误号。和线程不一样了
	sem_t类型，本质是结构体，应用时可简单看成整数，忽略实现，类似于谁的描述符
	sem_t sem; 规定信号量sem不能小于0，头文件#include<semaphore.h>

int sem_init(sem_t* sem, int pshared, unsigned int value);
	参二：是否进程间共享，0：线程间共享    非0：进程间共享
		//最好用宏：PTHREAD_PROCESS_SHARED PTHREAD_PROCESS_PRIVATE
	参三：N值，根据情况而定//决定了同时占用信号量的线程个数，理解成负载个数，当信号量->0满员了，post一个，+1，空一个位置
sem_wait---------
	1.信号量大于0，则信号--
	2.信号量等于0，造成信号阻塞
sem_post---------
	信号量++，唤醒阻塞在信号量上的线程
	但是sem_t的实现对用户隐蔽，所以++ --只能通过函数实现，不能用++ -符号

	
生产者消费者模型2：  //多生产者，多消费者，只有当极端条件才阻塞。并发性高
	线程->线程同步-> 生产者消费模型(必提)
	//用信号量
	1.前提：一个线性表(环形队列，不然性表乱拿会造成访问错误)，一个产品数信号量product 0，一个空格数信号量blank 5
	2.操作方法1：生产一个产品，表中产品++用sem_post(&product)，空格数--，用sem_wait(&blank)
		//一定要先放产品再把产品++，因为最初产品为0，拿它来阻塞用。
	2.操作方法2：拿走一个产品，表中产品--用sem_wait(&product)，空格数++，用sem_post(&blank)
		//先把产品--再拿产品，同样因为产品减少最多为0，拿来阻塞用    而空格在增加，不能拿来阻塞用。应该以减少的为中心

	
