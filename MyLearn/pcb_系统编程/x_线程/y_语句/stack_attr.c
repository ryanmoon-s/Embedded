#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
//设置栈大小，从堆空间拿
#define maxsize  0x10000000  //十六进制，比十进制空间更大
void* pthre(void* arg){
	while(1);
	sleep(1);
}

int main(){
	pthread_t tid;
	int ret;
	void* stackaddr;	
	int detac, count = 0;
	size_t size;
	pthread_attr_t attr; //新建
	pthread_attr_init(&attr); //初始化 未判断出错
	//读取状态
	pthread_attr_getdetachstate(&attr, &detac);
	pthread_attr_getstacksize(&attr, &size);
	if(PTHREAD_CREATE_DETACHED == detac){
		printf("thread detached\n");
	}else if(PTHREAD_CREATE_JOINABLE == detac){
		printf("thread joined\n");
	}else{
		printf("thread unknown\n");
	}
	printf("stacksize: %ld\n", size);

	//分离
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
	//循环创建线程
	while(1){
		stackaddr = malloc(maxsize);//堆上分配空间给栈
		if(NULL == stackaddr){
			perror("malloc");
			exit(1);
		}
		size = maxsize;
		pthread_attr_setstack(&attr, stackaddr, size);
		ret = pthread_create(&tid, &attr, pthre, NULL);
		if(ret != 0){
			fprintf(stderr, "pthread_create err: %s\n", strerror(ret));
			break;
		}
		printf("count = %d\n", ++count);
	}
	//销毁属性所占资源
	pthread_attr_destroy(&attr);
	return 0;
}
