#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>


void* pthre(void* arg){
	printf("thread num: %lu\n", pthread_self());
	return NULL;
}

int main(){
	pthread_t tid;
	int ret;
	pthread_attr_t attr; //新建
	pthread_attr_init(&attr); //初始化 未判断出错
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	ret = pthread_create(&tid, &attr, pthre, NULL);//传地址要遭，i在变
	if(ret != 0){
		fprintf(stderr, "pthread_create err: %s\n", strerror(ret));
	}
	ret = pthread_join(tid, NULL);  
	if(0 != ret){
		printf("join failed, err:%s\n", strerror(ret));//已经分离，回收失败
	}
	pthread_exit(NULL);
}
