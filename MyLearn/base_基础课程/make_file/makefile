#变量
obj=a.o add.o
target=app

#自己维护的变量
CC = gcc
CPPFLAGS = -I

#函数，匹配当前目录的字符串，挨个查找替换
src=$(wildcard ./*.c)
obj=$(patsubst ./%.c, ./%.o, $(src))

#终极目标
$(target):$(obj)
	gcc $(obj) -o $(target)

#次目标
%.o:%.c
	gcc -c $< -o $@

#生成主目标和这条没有关系，目标是clean，没有依赖，命令也不是用来生成目标的，这个用来删除临时文件
#用make clean 执行这条命令,上面的都不执行，-f强制执行，不管有没有.o文件
#如果文件夹中有clean了，就会提示：已是最新。clean为伪目标，还是会拿来和文件比较谁最新。用.PHONY:clean将其申明为伪目标，就不会再拿来比较了。
.PHONY:clean
clean:
#前面加个-，失败时继续向下执行，不加就不会执行
	-mkdir /aa
	rm $(obj) $(target) -f
	echo "have been cleand all the .o and target files"




