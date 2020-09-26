#include<stdio.h>
#include<stdlib.h>

int main(){
	char *name = "ABD";
	
	char* va1 = getenv(name);
	printf("%s = %s\n\n", name, va1);//没有值，返回NULL
	
	setenv(name, "hello", 1);//键，值，1：如果有就覆盖，0：不fugai
	
	va1 = getenv(name);
	printf("%s = %s\n\n", name, va1);//有值 ，输出值
	
	unsetenv(name);

	int ret = unsetenv("AAAA");
	printf("%d\n\n", ret);//取消一个不存在的，不报错，ret=0
	
	ret = unsetenv("AAAA=");
	printf("%d\n\n", ret);//取消一个包含内部格式的，报错，ret=-1
}
