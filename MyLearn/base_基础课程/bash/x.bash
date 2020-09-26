#!/bin/bash
#--上面第一行说明了是用的什么bash，不要改。一般都要先给执行权限再运行
#echo hello
#echo whorld


#--执行时带的参数，0为输入的第一个参数即文件名，1是第二个...
#echo $0
#echo $1

#--带参运用，输出信息到指定文件
#echo "information of itcas's computer:" >>$1
#uname -a>>$1    #用户信息
#free -h>>$1	#磁盘空间，h人类语言

#--返回值 ，正常退出返回0，可用exit自行设置，在外面用echo $? 来查看返回值 
#--程序会在exit处停止运行并返回
#exit 0

#--函数
#mytest(){
#	echo hello
#	echo world
#	echo $1
#}
#mytest #调用，直接名字

#--跨脚本调用 
#source y.bash   #执行到这行时会执行y.bash脚本
#myfun    #后续直接用里面的函数

#--逻辑判断  真返回0  假返回1 ，小于lt  等于eq  不等于ne  大于等于ge 小于等于le
#test -gt 2;echo $?   #外面输入   less than
#test -e a.out;echo $? #文件是否存在
#test -f a.out;echo $? #文件是否存在，且为普通文件
#test -d a.out;echo $? #文件是否存在，且为目录文件
#test -L a.out;echo $? #文件是否存在，且为软链接
#test -r a.out;echo $? #文件是否可读
#test -w a.out;echo $? #文件是否可写
#test -x a.out;echo $? #文件是否可执行
#复杂的  ! expre  expre1 -a expre2(and)   expre1 -o expre2(or) 

#--选择结构，if和[间有空格，结尾是fi  可以嵌套
#if [-e $1]
#then
#	echo "exit"
#else
#	echo "not exit"
#fi

#--case  in   中间用;; 结尾esac ，达成则break 
#--通配符：*任意文本 ?任意一个字符 []范围内一个字符[1-5][b-d]
#case $1 in
#x)
#	echo "x"
#;;
#y)
#	echo "y"
#;;
#*)
#	echo "other"
#esac

#--while循环  [后 ]前 while后都有空格 结尾done
#x=5
#while [ $x -gt 0 ]   #也可while true
#do
#	echo $x
#	x=$(($x-1))
#	sleep 1
#done

#--for循环
#for x in 1 2 3 10  #不加引号，个数就是运行次数
#do
#	echo $x
#	sleep 1
#done
#total=0
#for x in $(seq 1 1 100)  #要加$符号，不然无法识别 seq 等差数列，开始 加几 结束 
#do
#	total=$(($total+$x))
#done
#echo $total


