#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>
//strerror 把errno->错误的字符串
//fprintf 可以选择打印到哪去
void* thread_f(void* arg){
	printf("In thread: thread id = %lu, pid = %d\n", pthread_self(), getpid()); //进程id一样，线程id不同
	return NULL;
}

int main(){
	pthread_t tid;
	printf("In main1: thread id = %lu, pid = %d\n", pthread_self(), getpid());//调用别个的线程叫主控线程
	int ret = pthread_create(&tid, NULL, thread_f, NULL);  //线程直接去执行自己的函数
	if(ret != 0){
		fprintf(stderr, "pthread_create error:%s\n", strerror(ret));
		exit(1);
	}

	printf("In main2: thread id = %lu, pid = %d\n", pthread_self(), getpid());//调用别个的线程叫主控线程
	sleep(1);
	return 0;
}
