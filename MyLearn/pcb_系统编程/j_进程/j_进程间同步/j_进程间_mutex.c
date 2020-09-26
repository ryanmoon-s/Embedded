//------------------------------进程间Mutex----------------------------------
//锁要放在共享内存里面，它们共享这把锁，属性放在哪无所谓，只是给锁加配置，设置了锁就没用了。
//如果在fork之前设锁，不放在共享内存里面，分离后，复制了一份，他们有各自的锁，不是同一个锁，一个进程锁住后，
//另一个进程再Lock，也能锁住，根本不是同一把锁，没有意义
#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/mman.h>
void set_err(char* str){
	perror(str);
	exit(1);
}
typedef struct{ //锁要放在共享内存里面，和数据封装在一起，属性放在哪无所谓，只是拿来设置锁用
	int data;
	pthread_mutex_t mutex;
}data_t;
//全局变量不能通信，放弃
//不能用printf (虽然父子共享012文件描述符)两个进程有自己的输出缓冲区，刷新了才打印到屏幕上来，不是操作共享区域不需同步
//管道也不行，自带阻塞，不用mutex都可以同步   包括fifo
//可以用mmap共享映射区，两端自由访问同一片内存，需要同步，同样共享一把锁，用这把锁来实现同步
int main(){
	//建立共享映射区，把锁加载进来
	data_t* p;  //申请的映射地址存在p里面,参一填NULL是地址
	p = mmap(NULL, sizeof(data_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	if(p == MAP_FAILED){                             //注意anonymous是和shared位在一起的，后面fd传-1
		perror("mmap");
		exit(1);
	}
	p->data = 0;
	//初始化锁属性
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	//初始化锁
	pthread_mutex_init(&p->mutex, &attr);
	//创建子进程
	pid_t pid;
	pid = fork();
	if(pid == 0){
		pthread_mutex_lock(&p->mutex);
		for(int i = 0; i < 10; i++){
			sleep(1);
			p->data += 3;
			printf("child  %d\n", p->data);
		}
		pthread_mutex_unlock(&p->mutex);
	}else if(pid > 0){
		pthread_mutex_lock(&p->mutex);
		for(int i = 0; i < 10; i++){
			sleep(1);
			p->data += 100;
			printf("parent  %d\n", p->data);
		}
		pthread_mutex_unlock(&p->mutex);
		wait(NULL);
	}else{
		perror("fork");
		exit(1);
	}//父子进程自己删自己的
	pthread_mutex_destroy(&p->mutex);
	pthread_mutexattr_destroy(&attr);
	munmap(p, sizeof(data_t));
	return 0;
}
