#include "thread.h"
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>

//汇编码switch_to() 是错误的, 什么东西?
extern void switch_to (struct task_struct *next);

//线程上下文切换函数switch_to() (只能使用汇编版本[仅限x86 架构], 其他cpu 架构可能要重写汇编代码)
/*
void switch_to (struct task_struct *next){
	__asm__(
		//在c 语言中声明了switch_to() 函数, 汇编中不能再声明了;
		//".section .text\n"
		//".global switch_to\n"
		//"switch_to:\n\t"

		"call closealarm \n\t"   //调用函数closealarm

		"push %ebp \n\t"
		"mov %esp, %ebp \n\t"    //更改栈帧, 以便寻参 

		//保存现场 
		"push %edi \n\t"
		"push %esi \n\t"
		"push %ebx \n\t"
		"push %edx \n\t"
		"push %ecx \n\t"
		"push %eax \n\t"
		"pushfl \n\t"

		//准备切换栈 
		"mov current, %eax \n\t" //取 current 基址放到 eax 
		"mov %esp, 8(%eax) \n\t" //保存当前 esp 到线程结构体  
		"mov 8(%ebp), %eax \n\t" //8(%ebp)即为c语言的传入参数next, 取下一个线程结构体基址
		"mov %eax, current \n\t" //更新 current 
		"mov 8(%eax), %esp \n\t" //切换到下一个线程的栈 

		//恢复现场, 到这里, 已经进入另一个线程环境了, 本质是 esp 改变 
		"popfl \n\t"
		"popl %eax \n\t"
		"popl %ecx \n\t"
		"popl %edx \n\t"
		"popl %ebx \n\t"
		"popl %esi \n\t"
		"popl %edi \n\t"
		"popl %ebp \n\t"

		"call openalarm \n\t"    //调用函数openalarm

		//汇编式函数返回, 相当于c 语言的return ;
		"ret\n"
	);
	return ;
}
*/



static struct task_struct init_task = { 0, NULL, 0, THREAD_RUNNING, 0, 15, 15, {0} };

struct task_struct *current = &init_task;

struct task_struct *task[NR_TASKS] = { &init_task, };



// 线程启动函数
static void start (struct task_struct *tsk)
{
	tsk->th_fn ();
	tsk->status = THREAD_EXIT;
	printf ("thread [%d] exited\n", tsk->id);
	schedule ();
	// 下面这一行永远不会被执行
	printf ("thread [%d] th_resume\n", tsk->id);
}

int th_create (int *tid, void (*start_routine) (void))
{
	int *stack, id = -1;
	struct task_struct *tsk = (struct task_struct *) malloc (sizeof (struct task_struct));
	while (++id < NR_TASKS && task[id]) ;
	if (id == NR_TASKS)
		return -1;
	task[id] = tsk;
	if (tid)
		*tid = id;									//返回值
	tsk->id = id;
	tsk->th_fn = start_routine;
	stack = tsk->stack;						// 栈顶界限
	tsk->esp = (int) (stack + STACK_SIZE - 11);
	tsk->wakeuptime = 0;
	tsk->status = THREAD_STOP;
	tsk->counter = 15;
	tsk->priority = 15;
	// 初始 switch_to 函数栈帧
	stack[STACK_SIZE - 11] = 0;		// eflags
	stack[STACK_SIZE - 10] = 0;		// eax
	stack[STACK_SIZE - 9] = 0;		// edx
	stack[STACK_SIZE - 8] = 0;		// ecx
	stack[STACK_SIZE - 7] = 0;		// ebx
	stack[STACK_SIZE - 6] = 0;		// esi
	stack[STACK_SIZE - 5] = 0;		// edi
	stack[STACK_SIZE - 4] = 0;		// old ebp
	stack[STACK_SIZE - 3] = (int) start;	// ret to start 线程第一次被调度时会在此启动
	// start 函数栈帧, 刚进入 start 函数的样子 
	stack[STACK_SIZE - 2] = 100;					// ret to unknown, 如果 start 执行结束, 表明线程结束 
	stack[STACK_SIZE - 1] = (int) tsk;		// start 的参数

	//汇编函数调用,c风格参数传递; 传入参数分别是IP,c1,c2
	return 0;
}

void th_wait (int tid)
{
	if (task[tid] != NULL && task[tid]->status != THREAD_STOP && task[tid]->status != THREAD_EXIT)
	{
		task[tid]->status = THREAD_BLOCK;
	}
}

void th_resume (int tid)
{
	if (task[tid] != NULL && task[tid]->status != THREAD_STOP && task[tid]->status != THREAD_EXIT && task[tid]->status == THREAD_BLOCK)
	{
		task[tid]->status = THREAD_RUNNING;
	}
}

void th_detach (int tid)
{
	if (task[tid] != NULL && task[tid]->status == THREAD_STOP && task[tid]->status != THREAD_EXIT)
	{
		task[tid]->status = THREAD_RUNNING;
		schedule ();
	}
}

void wait_all (void)
{
	int i = 0;
	int remain = 0;
	while (1)
	{
		remain = 0;
		for (i = 1; i < NR_TASKS; i++)
		{
			if (task[i] && task[i]->status != THREAD_EXIT)
			{
				remain = 1;
				schedule ();
				break;
				continue;
			}
		}
		if (!remain)
		{
			break;
		}
	}
}

void th_wait4quit (int tid)
{
	while (task[tid] && task[tid]->status != THREAD_EXIT)
	{
		schedule ();
	}
}

void th_remove (int tid)
{
	if (task[tid])
	{
		free (task[tid]);						//释放空间
		task[tid] = NULL;
		printf ("the resource of thread %d is released\n", tid);
	}
}

void th_dispose (int tid)
{
	if (task[tid] != NULL)
	{
		task[tid]->status = THREAD_th_disposeD;
	}
}

int th_join (int tid)
{
	while (task[tid] && task[tid]->status != THREAD_EXIT)
	{
		if (task[tid]->status == THREAD_STOP)
		{
			task[tid]->status = THREAD_RUNNING;
		}
		schedule ();
	}
}





static unsigned int get_time_ms (void)
{
	struct timeval tv;
	if (gettimeofday (&tv, NULL) < 0)
	{
		perror ("gettimeofday()");
		//exit (-1);//弃用结束程序, 防止线城池崩溃
		return 0;
	}
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

//找到时间片最大的线程进行调度
static struct task_struct *pick (void)
{
	int i, next, c;
	for (i = 0; i < NR_TASKS; ++i)
	{
		if (!task[i])
			continue;

		if (task[i]->status == THREAD_EXIT)
		{
			if (task[i] != current)
				th_remove (i);
			continue;
		}

		if (task[i]->status == THREAD_th_disposeD)
		{
			if (task[i] != current)
				th_remove (i);
			continue;
		}

		if (task[i]->status != THREAD_STOP && task[i]->status != THREAD_BLOCK && get_time_ms () > task[i]->wakeuptime)
		{
			task[i]->status = THREAD_RUNNING;
		}
	}

	//上面的作用是唤醒睡眠的线程,使其可以接受调度
	while (1)
	{
		c = -1;
		next = 0;
		for (i = 0; i < NR_TASKS; ++i)
		{
			if (!task[i])
				continue;
			if (task[i]->status == THREAD_RUNNING && task[i]->counter > c)
			{
				c = task[i]->counter;
				next = i;
			}
		}

		if (c)
			break;

		// 如果所有任务时间片都是 0, 重新调整时间片的值
		if (c == 0)
		{
			for (i = 0; i < NR_TASKS; ++i)
			{
				if (task[i])
				{
					task[i]->counter = task[i]->priority + (task[i]->counter >> 1);
				}
			}
		}
	}
	return task[next];
}

void closealarm (void)
{
	sigset_t mask;
	sigemptyset (&mask);
	sigaddset (&mask, SIGALRM);
	if (sigprocmask (SIG_BLOCK, &mask, NULL) < 0)
	{
		perror ("sigprocmask() BLOCK");
	}
}

void openalarm (void)
{
	sigset_t mask;
	sigemptyset (&mask);
	sigaddset (&mask, SIGALRM);
	if (sigprocmask (SIG_UNBLOCK, &mask, NULL) < 0)
	{
		perror ("sigprocmask() BLOCK");
	}
}

void schedule (void)
{
	struct task_struct *next = pick ();
	if (next)
	{
		// 汇编语言switch_to.s 定义的函数 (debugging)
		switch_to (next);
	}
}

void th_sleep (int seconds)
{
	current->wakeuptime = get_time_ms () + 1000 * seconds;
	current->status = THREAD_SLEEP;
	schedule ();
}

static void do_timer (void)
{
	if (--current->counter > 0)
		return;
	current->counter = 0;
	schedule ();
	//printf("do_timer\n");//for test only
}

__attribute__((constructor)) static void init ()
{
	struct itimerval value;
	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = 1000;
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = 1000 * 10;// 10 ms
	if (setitimer (ITIMER_REAL, &value, NULL) < 0)
	{
		perror ("setitimer()");
	}
	signal (SIGALRM, do_timer);
}
