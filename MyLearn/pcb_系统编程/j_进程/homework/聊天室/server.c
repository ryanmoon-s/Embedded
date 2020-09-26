#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#define public_fifo "./publicfifo"
#define private_1 "./private_1"
#define private_2 "./private_2"
                                                                         //服务器
typedef struct{
	int sig;//0表示登录,1表示退出,2表示发送数据
	int log_id;//0.1.2共用
	int send_flag;//0对方不在线，1成功
	int source_id;//数据包专用
	int target_id;
	char data[50];
}com_data;

typedef struct{
	int num;
	int arr[10];
}log_table;

void set_err(char* str);
void table_init(log_table* table_t);//table初始化
void log_t(com_data* data_t, log_table* table_t, int flag_t); //flag:  0退出 1登录
void send_message(com_data* data_t, log_table* table_t);//发送数据 

int main(){
//	mkfifo(public_fifo, 0664);
//	mkfifo(private_1, 0664);
//	mkfifo(private_2, 0664);
	com_data data_t;
	log_table tab;
	table_init(&tab);
	int openf = open(public_fifo, O_RDONLY);
	if(openf == -1)
		set_err("open fifo");
	int read1;

	while(1){
		read1 = read(openf, &data_t, sizeof(data_t));  //阻塞监听
		if(read1 == -1)
			set_err("read1");
		if(read1 >= 0){
			switch(data_t.sig){
				case 0: log_t(&data_t, &tab, 1); break;
				case 1: log_t(&data_t, &tab, 0); break;
				case 2: send_message(&data_t, &tab); break;
				default: break;
			}
		}
	}
	return 0;
}

void set_err(char* str){
	perror(str);
	exit(1);
}

void table_init(log_table* table_t){
	table_t->num = 0;
	for(int i = 0; i < 10; i++){
		table_t->arr[i] = 0;
	}
}

void log_t(com_data* data_t, log_table* table_t, int flag_t){ 
	if(flag_t == 1){//登录
		table_t->arr[table_t->num] = data_t->log_id;
		table_t->num++;
	}else if(flag_t == 0){//退出
		for(int i = 0; i < table_t->num; i++){
			if(data_t->log_id == table_t->arr[i]){
				for(int j = i; j < table_t->num; j++){
					table_t->arr[j] = table_t->arr[j + 1];
				}//既然所有的都设为0了，那就直接后者覆盖前者，无需先覆盖后清零，    如果这样，那么最后一个元素，执行不到。(1能删除，在前面，2不能删除因为它在最后面)，所以循环次数要保证最后一个也能清零
			}
		}
	}
}



void send_message(com_data* data_t, log_table* table_t){ 
	int s = data_t->source_id;
	int t = data_t->target_id;
	int pri;
	//对方不在线
	data_t->send_flag = 0;
	for(int i = 0; i < table_t->num; i++){
		if(table_t->arr[i] == t){
			//对方在线
			data_t->send_flag = 1;
			break;
		}
	}
	//判断
	int tar;
	if(data_t->send_flag == 0){
		tar = s;
	}else if(data_t->send_flag == 1){
		tar = t;
	}
	//发送
	switch(tar){
		case 1:pri = open(private_1, O_WRONLY);	break;
		case 2:pri = open(private_2, O_WRONLY); break;
		}	
	if(pri == -1)
		set_err("open pri");
	int wr = write(pri, data_t, sizeof(com_data));  
	if(wr == -1)
		set_err("send message write");
	sleep(2);
}
