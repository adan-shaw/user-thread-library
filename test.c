#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>

//这并不是一个多线程程序, 而是一个普通实时定时器程序: ITIMER_REAL



void handler (int sig)
{
	static unsigned int wakeup_count = 0;
	if (sig == SIGALRM)
	{
		printf ("SIGALRM: %d\n", wakeup_count++);
	}
}

__attribute__((constructor)) void init (void)
{
	printf ("construct\n");
	if (signal (SIGALRM, handler) == SIG_ERR)
	{
		perror ("signal()");
	}
}

__attribute__((destructor)) void destory (void)
{
	printf ("destruct\n");
}

int main (void)
{
	//定义实时定时器
	struct itimerval value;
	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = 1000 * 20;
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = 1000 * 20;
	if (setitimer (ITIMER_REAL, &value, NULL) < 0)
	{
		perror ("setitimer()");
		return -1;
	}

	//死等定时器唤醒
	while (1)
		sleep (1);

	return 0;
}
