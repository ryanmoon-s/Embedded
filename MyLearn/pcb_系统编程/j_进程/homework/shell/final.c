#include<stdio.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>

void set_err(char* str);

int main(){
	char str[30]={0};    //能用数组的尽量用数组！！！！！！！
	int count = 0;
	char* strout[10];
	for(int i = 0; i < 10; i++){
		strout[i] = (char*)malloc(sizeof(char));
	}
#if 0
	慎用char *a="aaa";  aaa存放在常量区，后面再改变指针a的内容就会出错
	1.如果只是指针数组，后面去改变指针的指向，则所有的指针都指向了p
	2.但是数组里面全是指针，如果用strcpy是把p指向空间的内容复制过来，就没有地方存储。
	3.所以要为每一根指针开辟内存空间，直接char大小都不会出错，来存放strcpy复制过来的内容
#endif
	const char* split = " "; //分割符,要设为char*
	char* p=NULL;
	while(1){
		//接收命令
		scanf("%[^\n]", str);//不要直接%s，遇到空格会停止，用%[^\n]只能用一次？
		//分割命令
		p = strtok(str, split); //这个函数，参一第一次给它str，以后都给他NULL
		while(p != NULL){
			strcpy(strout[count], p);
			count++;
			p = strtok(NULL, split);
		}
		strout[count]=NULL; //最后一个是NULL，而不是字符串"NULL"!，如果有NULL了，拿来strcpy给其它的字符串变量要出错
		count ++;
		//创建进程
		pid_t pid;
		int i;
		for(i = 0; i < 1; i++){
			if((pid = fork()) == 0)
				break;
			if(pid == -1)
				set_err("fork");
		}
		switch (i){
			case 0:   //最后一个是NULL，不是"NULL"字符串
				execvp(strout[0], strout);
			default:
				for(int j = 0; j < i; j++)
					wait(NULL);
				//清空缓存
				memset(str, 0, 30);
				for(int j = 0; j < count; j++){
					free(strout[j]);
					strout[j]=(char*)malloc(sizeof(char));
				}
				setbuf(stdin, NULL);  //使stdin输入流由默认缓冲区转为无缓冲区，必须要做否则缓冲区中有一个\n
				p = NULL;
				count = 0;//千万不要释放strout，释放了又没内存放东西了！！！！！！！！！！！
				}
		
	}
	return 0;
}

void set_err(char* str){
	perror(str);
	exit(1);
}
