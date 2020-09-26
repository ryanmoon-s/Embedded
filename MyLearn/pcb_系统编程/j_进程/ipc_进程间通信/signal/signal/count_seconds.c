#include<stdio.h>
#include<unistd.h>

int main(){
	int i;
	alarm(1);
	for(int i = 0 ; ; i++){
		printf("%d\n", i);
	}
//	while(1);//验证了Linux下输出会在缓冲区，遇到\n才打印，不然一直等待，并且alarm终止不会打印出缓冲区数据
	return 0;
}
