#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>

void* pthre(void* arg){
	long i = (long)arg;
	sleep(i);
	printf("%ldth thread, thread num: %lu\n", i, pthread_self());
	return NULL;
}

int main(){
	pthread_t tid;
	int ret;
	long i;
	for(i = 1; i <= 5; i++){    //32位void* int都是4byte ，64位void*是8byte，高位加，高位截，但是我的编译器不允许，所以用long，我的电脑上是8byte
		ret = pthread_create(&tid, NULL, pthre, (void*)i);//传地址要遭，i在变
		if(ret != 0){
			fprintf(stderr, "pthread_create err: %s\n", strerror(ret));
		}
	}
	sleep(i);
	return 0;
}
