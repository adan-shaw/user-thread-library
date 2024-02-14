#ifndef __THREAD_H__
#define __THREAD_H__

#define NR_TASKS 1024
#define STACK_SIZE 1024					//1024*4 B

#define THREAD_READY 0
#define THREAD_RUNNING 1
#define THREAD_SLEEP 2
#define THREAD_BLOCK 3
#define THREAD_EXIT 4
#define THREAD_STOP 5
#define THREAD_th_disposeD 6

//thread info
struct task_struct
{
	int id;												//线程的标识符
	void (*th_fn) (void);					//指向线程函数的函数指针
	int esp;											//用来在发生线程切换是保存线程的栈顶地址
	unsigned int wakeuptime;			//线程唤醒时间
	int status;										//线程状态
	int counter;									//时间片
	int priority;									//线程优先级
	int stack[STACK_SIZE];				//现场的栈空间
};

//thread api
void th_wait4quit (int tid);
int th_create (int *tid, void (*start_routine) ());
int th_join (int tid);
void th_resume (int tid);
void th_wait (int tid);
void wait_all (void);
void th_sleep (int seconds);
void th_detach (int tid);
void th_remove (int tid);
void th_dispose (int tid);

#endif
