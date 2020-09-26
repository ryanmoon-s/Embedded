//子进程结束、暂停、继续时会向父进程发送SIGCHLD信号，默认忽略
//利用这个信号可以回收子进程，父进程就可以做自己的事。
//父进程写一个sigaction/signal，捕捉SIGCHLD，
父进程创建10个子进程，然后while(1);  屏蔽SIGCHLD +(注册捕捉SIGCHLD+解除屏蔽SIGCHLD)//sigemptyset自动解除
子进程退出向父进程发送SIGCHLD
处理函数里面回收子进程
父进程在执行某一个处理函数时，又死了几个子进程，只记录一次，有些就没被执行
if((pid == waitpid(0, &statius, WNOHANG)) > 0){
	if(WIFEXITED(status))
		printf("%d\n", WEXITSTATUS(status));
	else if(...)
		...
}
//只回收了一个，有几个白死了  0同组任意一个
while(...){
	
}//回收了一个后，又while判断，有->继续回收->再判断




//父进程与子进程用pipe实现  ls | wc -l  时，父进程exec执行了就走了，不能回收子进程
父进程dup2(fd[1], STDOUT_FILENO); 输出到管道     exec   
子进程dup2(fd[0], STDIN_FILENO);  从管道读值     exec
父进程注册捕捉SIGCHLDl回收子进程并打印...
//，好像不行，父进程直接走了，怎么回收
//交换，子进程ls，结果打进管道，死了发送SIGCHLD ，父进程回收并wc -l，但是这样不用信号也行

再者，捕捉函数里面的printf重定向到管道去了。
//直接父进程不用信号，直接wait，再wc -l  应该可以
