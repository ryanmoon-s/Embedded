//--------------------------多线程拷贝，有进度条--------------------------
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<pthread.h>
	//线程参数读取flag
int flag = 0;

	//线程参数结构体
typedef struct{
	int no;//线程编号
	int t_no;//总线程数
	int w_no;//写的字节数
	int last_jump;//最后一个线程跳过字节数
}info_t;

	//出错处理
void set_err(char* str){
	perror(str);
	exit(1);
}

	//拷贝线程
void* pthreads(void* arg){
	info_t* info = (info_t*)arg;
	flag = 1;//读入数据，flag置1
	//计算跳过字节
	int jump;
	if(info->no != info->t_no){
		jump = (info->no - 1)*info->w_no;
	}else{
		jump = info->last_jump;
	}
	//独自打开文件，防止多线程混乱
	int f1 = open("123.txt", O_RDONLY);//源
	int f2 = open("321.txt", O_WRONLY);//目标
	//拷贝
	char* buff = (char*)malloc(info->w_no);
	int lseek1 = lseek(f1, jump, SEEK_SET);
	int lseek2 = lseek(f2, jump, SEEK_SET);
	if((-1 == lseek1)|(-1 == lseek2))
		set_err("lseek");
	int readd = read(f1, buff, info->w_no);
	if(-1 == readd)
		set_err("read");
	int writed = write(f2, buff, info->w_no);
	if(-1 == writed)
		set_err("write");
	pthread_exit(NULL);
}

	//进度条线程
void* typest(void* arg){
	int* tnum = (int*)arg;//线程数
	int sleeps = (int)(140000/(*tnum));
	char proc[101];
	char* ba = "|/-\\";  //   两个\转义\,两个%转义% ,
	for(int i = 0; i <= 100;){//打印100次
		printf("[%-100s][%d%%][%c]\r", proc, i, ba[i%4]); //\r是回车，回到本行开始，会覆盖   \n是换行，到下一行
		fflush(stdout);  //不用\n 只能flush刷新输出缓冲区到屏幕
		proc[i++] = '#';
		proc[i] = '\0';
		usleep(sleeps);
	}
	printf("\ncomplete!\n");
}

int main(int argc, char* argv[]){   //线程个数
	if(argc < 2){
		printf("./out thread_num\n");
		exit(1);
	}
	const int thread_num = atoi(argv[1]);  //此值作为传入参数，后面不能改变，用const修饰
	//启动进度条线程
	pthread_t type_tid;
	pthread_create(&type_tid, NULL, typest, (void*)&thread_num);
	//------------分配拷贝资源----------------
	int fd1 = open("123.txt", O_RDONLY);//源
	if(-1 == fd1)
		set_err("open file");
	//获取源文件大小
	struct stat sta;
	int stat_ret = fstat(fd1, &sta);
	if(-1 == stat_ret)
		set_err("stat");
	int size_file = sta.st_size;
	//扩展目标文件
	int fd2 = open("321.txt", O_CREAT|O_WRONLY, 0644);//目标
	if(-1 == fd2)
		set_err("open file");
	int ftr = ftruncate(fd2, size_file);
	if(-1 == ftr)
		set_err("ftruncate");
	//分配写的字节数
	int *arrange = (int*)malloc((thread_num)*sizeof(int)); //前面存跳过的字节数，最后一格存最后一个需要写的个数
	int any = size_file/thread_num;
	for(int i = 0; i < thread_num; i++){
		if((thread_num - 1) != i){
			arrange[i] = any;
		}else{
			arrange[i] = size_file - i*any;
		}
	}
	info_t info;
	info.t_no = thread_num;
	//创建拷贝线程
	pthread_t copy_tid;
	for(int i = 0; i < thread_num; i++){
		info.no = i+1; //结构体是指针传递，成员是值传递。只要不在主函数改变结构体值，就不会出错。用结构体是个好办法。
		info.w_no = arrange[i];//想要外面不能改变结构体，只能const修饰，但是又需要一次把值赋完，难搞。。。。
		if(i == thread_num - 1)
			info.last_jump = i*arrange[0];
		copy_tid = pthread_create(&copy_tid, NULL, pthreads, (void*)&info);
		if(0 != copy_tid){
			fprintf(stderr, "copy thread err:%s\n", strerror(copy_tid));
			exit(1);
		}
		while(0 == flag);//阻塞等待线程将变量读入栈空间         信号不可这样控制，会时序混乱
		flag = 0;
		pthread_detach(copy_tid); //分离线程
	}
	pthread_exit(NULL);
}
