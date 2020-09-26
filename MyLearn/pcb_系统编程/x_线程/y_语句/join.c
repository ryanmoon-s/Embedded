#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>

typedef struct{
	int a;
	int b;
}exit_t;

void* pthre(void* arg){
	exit_t *ex = (exit_t*)malloc(sizeof(exit_t));
	ex->a = 100;
	ex->b = 200;
	printf("thread num: %lu\n", pthread_self());
	pthread_exit((void*)ex);
}

int main(){
	pthread_t tid;
	int ret;
	exit_t *ex;
	ret = pthread_create(&tid, NULL, pthre, NULL);//传地址要遭，i在变
	if(ret != 0){
		fprintf(stderr, "pthread_create err: %s\n", strerror(ret));
	}
								//指针再取地址，二重指针，才能转换成void**
	pthread_join(tid, (void**)&ex);  //传出参数为void*，要把ex转换成void**来接收返回参数。所以要用指针类型转换成void**
	printf("a = %d, b = %d\n", ex->a, ex->b);
	return 0;
}
