#include "thread.h"
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>

void schedule ();

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
	printf ("thread [%d] resume\n", tsk->id);
}

int thread_create (int *tid, void (*start_routine) ())
{
	int id = -1;
	struct task_struct *tsk = (struct task_struct *) malloc (sizeof (struct task_struct));
	while (++id < NR_TASKS && task[id]) ;
	if (id == NR_TASKS)
		return -1;
	task[id] = tsk;
	if (tid)
		*tid = id;									//返回值
	tsk->id = id;
	tsk->th_fn = start_routine;
	int *stack = tsk->stack;			// 栈顶界限
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
	stack[STACK_SIZE - 3] = (int) start;	// ret to start   线程第一次被调度时会在此启动
	// start 函数栈帧，刚进入 start 函数的样子 
	stack[STACK_SIZE - 2] = 100;	// ret to unknown，如果 start 执行结束，表明线程结束 
	stack[STACK_SIZE - 1] = (int) tsk;	// start 的参数

	/*
	   汇编函数调用,c风格参数传递
	   传入参数分别是IP,c1,c2
	 */
	return 0;
}

void wait (int tid)
{
	if (task[tid] != NULL && task[tid]->status != THREAD_STOP && task[tid]->status != THREAD_EXIT)
	{
		task[tid]->status = THREAD_BLOCK;
	}
}

void resume (int tid)
{
	if (task[tid] != NULL && task[tid]->status != THREAD_STOP && task[tid]->status != THREAD_EXIT && task[tid]->status == THREAD_BLOCK)
	{
		task[tid]->status = THREAD_RUNNING;
	}
}

void detach (int tid)
{
	if (task[tid] != NULL && task[tid]->status == THREAD_STOP && task[tid]->status != THREAD_EXIT)
	{
		task[tid]->status = THREAD_RUNNING;
		schedule ();
	}
}

void wait_all ()
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

void wait_thread (int tid)
{
	while (task[tid] && task[tid]->status != THREAD_EXIT)
	{
		schedule ();
	}
}

void remove_th (int tid)
{
	if (task[tid])
	{
		free (task[tid]);						//释放空间
		task[tid] = NULL;
		printf ("the resource of thread %d is released\n", tid);
	}
}

void dispose (int tid)
{
	if (task[tid] != NULL)
	{
		task[tid]->status = THREAD_DISPOSED;
	}
}

int thread_join (int tid)
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
