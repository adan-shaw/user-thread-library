#include"thread.h"
#include<stdio.h>

void fun1 (void)
{
	int i = 0;
	for (i = 0; i < 10; i++)
	{
		printf ("thread id=%d time=%d\n", 1, i);
		th_sleep (2);
	}
}

void fun2 (void)
{
	int i = 0;
	for (i = 0; i < 10; i++)
	{
		printf ("thread id=%d time=%d\n", 2, i);
		if (i == 4)
		{
			th_wait (1);
			printf ("wait thread 1 \n");
		}
		th_sleep (1);
		if (i == 6)
		{
			th_resume (1);
			printf ("th_resume thread 1 \n");
		}
	}
}

void fun3 (void)
{
	int i = 0;
	for (i = 0; i < 10; i++)
	{
		printf ("thread id=%d time=%d\n", 3, i);
		th_sleep (3);
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
