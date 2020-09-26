终端：所有输入输出设备的总称

Alt + Ctrl + F1 F2 F3 ... F7 字符终端  pts(pseudo terminal slave)指伪终端

Alt + F7 图形终端

SSH Telnet 网络终端

--------------------------------------------------------------------------

终端启动流程：ps ajx
	init(1) -> fork -> exec -> getty -> 用户输入账号 -> login -> 输入密码 -> exec -> bash(文字终端)
	
(ctrl + c 产生信号 -> 线路规程...)

ttyname函数{
	由文件描述符查出对应的文件名
	char* ttyname(int fd); 成功：终端名  失败：NULL，设置ERRNO
	printf("%s\n", ttyname(0));
}

网络终端
	SSH客户端 <-> TCP/IP协议栈 <-> 网络设备驱动 <-> 网络
	网络 <-> 网络设备骚动 <-> TCP/IP协议栈 <-> SSH服务器 <->伪终端主设备 <->伪终端从设备 <-> 线路规程 <-> read/write系统调用 <-> bash


