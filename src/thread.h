#ifndef __THREAD_H__
#define __THREAD_H__

#define NR_TASKS 1024
#define STACK_SIZE 1024					// 1024*4 B

#define THREAD_READY 0
#define THREAD_RUNNING 1
#define THREAD_SLEEP 2
#define THREAD_BLOCK 3
#define THREAD_EXIT 4
#define THREAD_STOP 5
#define THREAD_DISPOSED 6

struct task_struct
{
	int id;												//线程的标识符
	void (*th_fn) ();							//指向线程函数的函数指针
	int esp;											//用来在发生线程切换是保存线程的栈顶地址
	unsigned int wakeuptime;			// 线程唤醒时间
	int status;										// 线程状态
	int counter;									// 时间片
	int priority;									// 线程优先级
	int stack[STACK_SIZE];				//现场的栈空间
};

void wait_thread (int tid);
int thread_create (int *tid, void (*start_routine) ());
int thread_join (int tid);
void resume (int tid);
void wait (int tid);
void mysleep (int seconds);
void detach (int tid);
void remove_th (int tid);
void dispose (int tid);

#endif
