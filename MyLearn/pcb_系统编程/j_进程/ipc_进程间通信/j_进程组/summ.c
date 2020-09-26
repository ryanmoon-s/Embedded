为了简化对多个进程的管理

父进程创建多个子进程时，进程组id=第一个进程id，组长进程的id = 进程组id，父进程是组长
//使用 kill -SIGKILL -1000 杀死整个组的全部进程
组长创建进程后死了，进程也在，只要组内有一个进程还在，组就在。
//进程组生存期：最后一个进程离开(终止或者转移到其它进程)
一个进程可以为自己或子进程设置进程组id

进程组操作函数：
getpgrp: get porcess group
	pid_t getpgrp(void);总是返回调用者的进程组id

getpgid: get process group id
	pid_t getpgid(pid_t pid); 成功0 失败-1 errno 获取指定进程的进程组id
	pid = 相当于 getpgrp

setpgid: set process group id
	int setpgid(pid_t pid, pid_t pgid); 成功0 失败-1 err
	改变进程默认所属进程组，通常可用来加入一个现有的进程组 创建一个新的进程组
	将参一的进程，加入参二对应的组中
	注意：
		1.如改变子进程为新组，应该在fork后，exec前
		2.权级问题，非root进程只能改变自己创建的子进程，或者有权限操作的进程


