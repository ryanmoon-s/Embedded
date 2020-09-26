#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

//条件变量生产消费者模型

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //互斥锁 静态初始化
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; //条件变量 静态初始化
typedef struct bing{ //链表模拟一张饼
	struct bing* next;   //引用本结构体，要在上面也写名称，这里要加struct
	int size;
}bing;
bing* head;


void* producer(void* arg){
	bing *big;
	while(1){
		sleep(rand()%2);
		//生产
		big = (bing*)malloc(sizeof(bing));
		big->size = rand() % 100+1;
		//加锁
		pthread_mutex_lock(&mutex);
		big->next = head->next;
		head->next = big;//头插法
		printf("生产了一张饼，大小:%d\n",big->size);
		pthread_mutex_unlock(&mutex);
		//通知
		pthread_cond_broadcast(&cond);
	}
}
void* comsumer(void* arg){
	bing* mine;
	while(1){
		pthread_mutex_lock(&mutex);
		while(head->next == NULL){//没有饼才来等待，有饼直接后面去取饼，如果没法有这个语句，明明有饼，还在这阻塞，
			pthread_cond_wait(&cond, &mutex);                             //只有等师傅再生产一张，发信号了，才去吃
		}
		//取饼，删链操作，为方便，从head后面一个取
		mine = head->next;
		printf("吃了一张饼，大小:%d\n", mine->size);
		head->next = head->next->next;
		free(mine);
		mine = NULL;
		pthread_mutex_unlock(&mutex);//不加的话就自己加锁两次了，死锁
		sleep(rand()%2);
	}
}

int main(){
	head = (bing*)malloc(sizeof(bing));
	head->next = NULL;
	srand(time(NULL));
	pthread_t tid1, tid2;
	pthread_create(&tid1, NULL, producer, NULL);//生产者
	pthread_create(&tid2, NULL, comsumer, NULL);//消费者

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
}
