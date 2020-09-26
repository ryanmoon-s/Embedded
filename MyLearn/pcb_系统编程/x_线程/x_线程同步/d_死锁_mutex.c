#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
//实现一个死锁
//主要是为了和平访问对方锁里面的文件，而去拿对方的锁。如果不拿锁也能访问的。
//不是说锁住了，编译器就能识别里面的内容，不让别人访问，而是为了不乱，我知道你有那个文件，我取锁再访问，为了和你错开
//主要是拿锁。，和里面文件关系不大
pthread_mutex_t mutex_a, mutex_b;

void* func(void* arg){
	pthread_mutex_init(&mutex_b, NULL);
	sleep(1);
	pthread_mutex_lock(&mutex_b);
	printf("b locked b\n");
	pthread_mutex_lock(&mutex_a);
	printf("b locked a\n");
	pthread_mutex_unlock(&mutex_a);
	pthread_mutex_unlock(&mutex_b);
}

int main(){
	pthread_mutex_init(&mutex_a, NULL);
	pthread_t tid;
	pthread_create(&tid, NULL, func, NULL);
	pthread_mutex_lock(&mutex_a);
	printf("a locked a\n");
	sleep(2);
	pthread_mutex_lock(&mutex_b);
	printf("a locked b\n");
	pthread_mutex_unlock(&mutex_b);
	pthread_mutex_unlock(&mutex_a);


	return 0;
}
