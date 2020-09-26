//获取指定目录下的普通文件个数  /home/itcas/func_system

#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>

int getFileNum(char* root);

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("./a.out path");
		exit(1);
	}
	int num = getFileNum(argv[1]);
	printf("ordinary file number: %d\n", num);
}

int getFileNum(char* root){
	//open dir
	DIR* dir= NULL;
	dir = opendir(root);
	if(dir == NULL){
		perror("opendir");
		exit(1);
	}
	//遍历
	char path[1024] = {0}; //中途遇到目录
	int total = 0;//总目录数
	struct dirent* ptr = NULL;
	while((ptr = readdir(dir)) != NULL){
		//排除 . 和 ..
		if(strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0){
			continue;
		}
		//遇到了目录
		if(ptr->d_type == DT_DIR){
			//将名字拼进原路径，递归读目录
			sprintf(path, "%s/%s", root, ptr->d_name);
			total += getFileNum(path); //total+下个文件里面的普通文件个数
		}
		//遇到了普通文件
		if(ptr->d_type == DT_REG){
			total++;
		}
	}
	closedir(dir);//必须关掉文件，不然文件个数为0
	return total;
}


