#include <stdio.h>
#include <sys/syscall.h>
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
//#include <linux/uaccess.h>
//_syscall2(int,hello,int,flag)
int main(){
	char buf[20];
	size_t len=18;
	int ret=syscall(548,buf,len);
	//printf("%s %ld %d",buf,len,ret);
	if(ret==-1) printf("-1\n");
	else printf("%s\n",buf);
	return 0;
}
