#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
//测试电脑能创建多少个线程

void* pthre(void* arg){
	while(1);
}

int main(){
	pthread_t tid;
	int ret;
	int count = 0;
	for(;;){
		ret = pthread_create(&tid, NULL, pthre, NULL);//传地址要遭，i在变
		if(ret != 0){
			fprintf(stderr, "pthread_create err: %s\n", strerror(ret));
			break;
		}
		printf("count = %d\n", count++);
	}
	return 0;
}
