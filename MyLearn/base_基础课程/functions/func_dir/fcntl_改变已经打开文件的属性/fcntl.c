//以读的方式打开，突然又想写了，不需要重新打开一次文件
//五大功能，主要:获得设置文件状态 int fcntl(int fd, int cmd, long arg) 
//cmd:F_GETFL F_SETFL  ，获取时p3置零，可设置：O_APPEND 追加，O_NOBLOCK非阻塞

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string,h>

//以只写方式打开
...
//覆盖的方式写
...
//获取当前状态
int flag = fcntl(fd, F_GETFL, 0);
//按位或上O_APPEND，相当于加将APPEND加入到原flag里面   111011  |  000100  ->  111111
flag |= O_APPEND;
//写入更改后的状态
fcntl(fd, F_SETFL, flag);
//追加的方式写
...
//close
