#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<semaphore.h>
#include<pthread.h>
//信号量生产消费者模型

#define maxnum 10 //队列最大容量
typedef struct queue{
	int line[maxnum];  //这个模型信号量大小取决于队列容量，同样决定最多同时容纳的线程数
	int head; //头进
	int tail; //尾出
}queue;
queue* que; //全局队列
sem_t blank, star; //全局信号量
void insert(int var);
int del();

void* producer(void* arg){
	long id = (long)arg;
	if(id > 0)  //单线程打印
		pthread_exit(NULL);
	int prod;  //这个一定要放在while外面定义，不然每循环一次又定义一次
	int in = 1;//顺序生产
	while(1){
		//生产
		prod = rand() % 100 +1;//随机生产
		sem_wait(&blank);//-- 生产者的阻塞条件是空格为0，没地方放产品了，所以以空格来阻塞，当减为0时，阻塞其它线程
		//insert(prod);
		insert(in);
		printf("%ld produce, size = %d\n", id, in);
		sem_post(&star);//++  先放进去，再产品++，在这之前产品为0，消费者会阻塞等待
		in++;//不能insert(in++),in++了后面打印的就是比生产的大1的了
		usleep(1000); //让cpu能易主
	}
}
void* comsumer(void* arg){
	long id = (long)arg;
	int prod;
	while(1){            //消费者的阻塞条件是产品为零，以产品来阻塞。当其为1时，相当于先加mutex再访问
		sem_wait(&star);//--如果没有产品则阻塞，如果有一个产品则先产品--让其等于0，其它消费者就会阻塞等待。
		prod = del(); //因为都是从队尾取的值，所以一定是按顺序取出来的，但是取和打印中间可能失去cpu，打印顺序可能不样
		printf("%ld get, size = %d\n", id, prod);
		sem_post(&blank);//++
		usleep(20000);
	}
}

int main(){
	srand(time(NULL));
	//信号量初始化
	sem_init(&star, 0, 0);
	sem_init(&blank, 0, maxnum);
	//队列初始化
	que = (queue*)malloc(sizeof(queue));
	for(int l = 0; l < maxnum; l++){
		que->line[l] = 0;
	}
	que->head = 0;
	que->tail = 0;
	//3个线程生产，3个线程消费
	pthread_t tid[6];
	long i, j;
	for(i = 0; i < 3; i++){
		pthread_create(&tid[i], NULL, producer, (void*)i);
	}
	for(j = 3; j < 6; j++){
		pthread_create(&tid[j], NULL, comsumer, (void*)j);
	}
	for(int k = 0; i < 6; i++){
		pthread_join(tid[k], NULL);
	}
	sem_destroy(&blank);
	sem_destroy(&star);
	
	return 0;	
}

void insert(int var){
	que->head = (que->head + 1) % maxnum;
	que->line[que->head] = var;
}
int del(){
	int temp = 0;
	//信号量不允许删完了再有线程删它
	//tail指着的不是出来的值，后面一个才是
	que->tail = (que->tail +1) % maxnum;
	temp = que->line[que->tail]; 
	que->line[que->tail] = 0;
//	if(que->tail > que->head)  循环队列不能这样判断，当循环完一个流程后，尾部肯定会比头部大
//		printf("e--------------------------------------------%d\n", temp);
	return temp;
}
