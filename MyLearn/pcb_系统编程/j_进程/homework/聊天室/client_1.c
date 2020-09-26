#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#define public_fifo "./publicfifo"
#define private_1 "./private_1"
void set_err(char* str);
                                                        //1号客户端
typedef struct{
	int sig;//0表示登录,1表示退出,2表示发送数据
	int log_id;//0.1.2共用
	int send_flag;//0对方不在线，1成功
	int source_id;//数据包专用
	int target_id;
	char data[50];
}com_data;

int main(){
	com_data send_data;
	com_data* rece_data;
	int cmmd;
	int isin = 0;//1在线 0离线
	int read1;
	int pid;
	int fi;
	for(fi = 0; fi < 1; fi++){
		if((pid = fork()) == 0)
			break;
		if(pid == -1)
			set_err("fork");
	}

	if(fi ==  0){//子进程
		int pri = open(private_1, O_RDONLY);
		while(1){
			//接收数据
			rece_data = (com_data*)malloc(sizeof(com_data));
			read1 = read(pri, rece_data, sizeof(com_data));  //阻塞监听
			if(read1 == -1){
				set_err("read1");
			}else if(read1 > 0){
				//收到数据，解析
				if(rece_data->send_flag == 1){  //必须加\n， 1 linux下遇到\n会打印到屏幕，2 程序结束，3 执行输入函数时，4 缓冲区满， 5 用fflush刷新
					printf("收到 %d 发来的消息: %s\n", rece_data->source_id, rece_data->data);
				}else if(rece_data->send_flag == 0){
					printf("发送失败，对方不在线！\n");
				}
				//清除缓存
				free(rece_data);
				rece_data = NULL;
			}
		}
	}
	if(fi == 1){//父进程
		int openf = open(public_fifo, O_WRONLY);
		int wr;
		if(openf == -1)
			set_err("open fifo");
		while(1){
			//发送数据
			if(isin == 0){
				printf("当前状态:离线  请输入指令:1登录\n");
				scanf("%d", &cmmd);
				setbuf(stdin, NULL);
				if(cmmd == 1){
					send_data.sig = 0; 
					send_data.log_id = 1;
					isin = 1;	
					cmmd = 9;
				}
			}else if(isin == 1){
				printf("当前状态:在线  请输入指令:1退出登录 2发送消息\n");
				scanf("%d", &cmmd);
				setbuf(stdin, NULL);
				switch (cmmd){
					case 1:
						send_data.sig = 1;
						send_data.log_id = 1;
						isin = 0;
						cmmd = 9;
						break;
					case 2:
						printf("请输入要发送的消息：\n");
						scanf("%[^\n]", send_data.data);
						setbuf(stdin, NULL);
						send_data.sig = 2;
						send_data.source_id = 1;
						send_data.target_id = 2;
						break;
						cmmd = 9;
					default:break;
				}
			}
			//发送数据
			wr = write(openf, &send_data, sizeof(send_data));
			//清除缓存
			memset(&send_data, 0, sizeof(send_data));
		}
	}
	return 0;
}

void set_err(char* str){
	perror(str);
	exit(1);
}
