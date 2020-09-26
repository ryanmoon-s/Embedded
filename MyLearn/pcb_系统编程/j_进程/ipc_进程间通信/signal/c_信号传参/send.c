》》》》》》》发送信号传参 》》》》》》》》》》》》》》》》》》》
一般不用信号传参数，如果非有这种要求可以这样
-----对应kill函数----发送信号------
int sigqueue(pid_t pid, int sig, const union sigval value);//成功0，失败-1 error
union sigval{
	int sigval_int;
	void* sigval_ptr;
}
在联合体里面携带数据，但是不同进程的虚拟地址各自独立，传送当前进程的地址  没有任何意义
* 用于给本进程发送信号:qt 的按钮，写成回调，函数写在那里，点击才调
》》》》》》》sigaction 接收参数》》》》》》》》》》》》》》》》》》》
用函数指针 (sa_sigaction) (int, siginfo_t*, void*)  并且flags要设为SA_SIGINFO
siginof_t   很多信息：
哪个进程给我发的、... ...
