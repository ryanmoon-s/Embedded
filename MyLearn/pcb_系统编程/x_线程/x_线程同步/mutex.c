#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
//一个线程输出 hello ... 一个大写，会混乱 未同步
//忽略检查出错 
//保护共享数据，锁的粒度越小越好，在将要访问的数据包起来
//不是说包起来锁住了别人就不能访问了，别人直接就可以访问，只是对方讲礼，知道这片区域的东西是你在访问，所以来要这片
//区域的锁，要到了再访问。
pthread_mutex_t mutex; //定义锁
void* func(void* arg){
	while(1){
		sleep(1);
		pthread_mutex_lock(&mutex); //锁
		printf("hello");
		sleep(1);
		printf("world\n");
		pthread_mutex_unlock(&mutex);//解锁
	}
	pthread_exit(NULL);
}

int main(){
	int	flag = 5;
	pthread_t tid;
	pthread_mutex_init(&mutex, NULL);//初始化锁 mutex == 1
	pthread_create(&tid, NULL, func, NULL);
	while(flag--){
		sleep(1);    //不要顶部加锁，底部取锁。  另一个等不到锁
		pthread_mutex_lock(&mutex);
		printf("HELLO");
		sleep(1);
		printf("WORLD\n");
		pthread_mutex_unlock(&mutex);
	}
	pthread_cancel(tid);
	pthread_join(tid, NULL);
	pthread_mutex_destroy(&mutex);
	return 0;
}
