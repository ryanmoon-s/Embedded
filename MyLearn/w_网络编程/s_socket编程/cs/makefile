src=$(wildcard *.c)                #所有.c文件存到src里面
obj=$(patsubst %.c, %.o, $(src))   #src中所有.c文件->.o文件

all: server client           #这样可不打包成一个，分开打包，所有主目标

server: server.o wrap.o 
	gcc server.o wrap.o -o server -Wall     #-Wall 编译后显示所有警告
client: client.o wrap.o 
	gcc client.o wrap.o -o client -Wall     #-Wall 编译后显示所有警告

%.o:%.c
	gcc -c $< -Wall

.PHONY: clean  #不算目标，加了.PHONY不会将clean拿来比较旧新
clean:
	-rm -rf server client $(obj)
