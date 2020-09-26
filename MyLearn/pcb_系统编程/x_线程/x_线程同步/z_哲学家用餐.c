#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
/*
  0  1  2  3  4 
  A  B  C  D  E
0  1   2  3  4

 */
//五把锁->筷子
pthread_mutex_t mutex[5];
//哲学家的左手右手
typedef struct{
	long left;
	long right;
}hands;
void* thinker(void* arg){
	long id = (long)arg;
	hands hand;
	//设置左手右手拿的锁的下标
	if(id == 4){ //最后一个反着拿
		hand.left = 0;
		hand.right = id;
	}else{//其它先拿左手
		hand.left = id;
		hand.right = id + 1;
	}
	//拿锁
	while(1){
		pthread_mutex_lock(&mutex[hand.left]);
		if(pthread_mutex_trylock(&mutex[hand.right]) != 0){ //尝试拿右手
			//失败，放弃左手
			pthread_mutex_unlock(&mutex[hand.left]);
			sleep(1);
		}else{
			//成功，吃饭，然后放右手，放左手
			printf("%ldth have the noodles, exit!\n", id);
			pthread_mutex_unlock(&mutex[hand.right]);
			pthread_mutex_unlock(&mutex[hand.left]);
			break;
		}
	}
	pthread_exit(NULL);
}

int main(){
	//初始化锁
	for(int i = 0; i < 5; i++){
		pthread_mutex_init(&mutex[i], NULL);
	}
	//创建线程
	pthread_t tid[5];
	for(long i = 0; i < 5; i++){
		pthread_create(&tid[i], NULL, thinker, (void*)i);
	}
	//释放线程
	for(int i = 0; i < 5; i++){
		pthread_join(tid[i], NULL);
	}
	//销毁锁
	for(int i = 0; i < 5; i++){
		pthread_mutex_destroy(&mutex[i]);
	}
	return 0;
}



#if 0
	一盘面，周围几个哲学家ABCDE，所有哲学家之间都有一根筷子，哲学家要拿到左手和右手的筷子(一双)才能吃
	哲学家对应的是线程，1 2 3 4 5根筷子是锁，面是共享资源
	死锁(震荡)：一开饭，所有哲学家都先拿着自己左手的筷子再去抢右手的筷子，死锁了。他们都放弃自己的筷子，又都拿起了自己左手的筷子，震荡了
	解决：让其中一个哲学家逆着拿筷子，他的一边的人没筷子，另一边的人就可以拿两根筷子，吃了放下，吃货旁边的人也能吃了

	常规避免死锁的方法：
		1.得不到资源时，放弃已有的资源
		2.每个线程获取资源的顺序一致，A取1 2 3 ，B也应该取1 2 3 。但这样遇见哲学家吃饭问题时，又死锁了
			//所以引入了哲学家吃饭问题的解决办法
#endif
