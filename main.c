#include<stdio.h>
#include"thread.h"

//这种小型线城池, 就不要太挑剔了, 只适合小型系统, 没有太多信息传递
/*
	传递/共享数据, 主要靠全局变量, 线程函数是不传递数据的, 简易线城池;
	线程函数的统一规格是:
		void (*start_routine) (void);

	这意味着:
		共享数据, 都是靠全局变量, 这样的话, 一般都是单进程小程序模式, 大型模式请用posix thread / c++ thread
		本线城池, 只适用于嵌入式小系统;

	迁移/移植建议:
		不同操作系统中, struct task_struct *tsk; 的内容定义可能都不一样, switch_to() 的的内容定义可能都不一样;
		因此, 要做迁移/移植, 要看看th_create() / switch_to() 的实现, 是否能编译通过, 能无缝迁移, 也是一件好事;

	本测试平台:
		x86 32bit/64bit intel-linux
*/



#define th_print_count (32)
#define th_print_sleep_count (1)

void fun1 (void)
{
	int i = 0;
	for (i = 0; i < 10; i++){
		printf ("thread id=%d time=%d\n", 1, i);
		th_sleep (th_print_sleep_count);
	}
}

void fun2 (void)
{
	int i = 0;
	for (i = 0; i < 10; i++){
		printf ("thread id=%d time=%d\n", 2, i);
		if (i == 4){
			th_wait (1);
			printf ("wait thread 1 \n");
		}
		th_sleep (1);
		if (i == 6){
			th_resume (1);
			printf ("th_resume thread 1 \n");
		}
	}
}

void fun3 (void)
{
	int i = 0;
	for (i = 0; i < 10; i++){
		printf ("thread id=%d time=%d\n", 3, i);
		th_sleep (th_print_sleep_count);
	}
}

int main (void)
{
	int th1, th2, th3;
	th_create (&th1, fun1);
	th_create (&th2, fun2);
	th_create (&th3, fun3);
	th_detach (th2);
	th_detach (th3);
	th_join (th1);
	return 0;
}
