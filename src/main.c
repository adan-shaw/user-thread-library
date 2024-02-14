#include"thread.h"
#include<stdio.h>

void fun1 ()
{
	int i = 0;
	for (i = 0; i < 10; i++)
	{
		printf ("thread id=%d time=%d\n", 1, i);
		mysleep (2);
	}
}

void fun2 ()
{
	int i = 0;
	for (i = 0; i < 10; i++)
	{
		printf ("thread id=%d time=%d\n", 2, i);
		if (i == 4)
		{
			wait (1);
			printf ("wait thread 1 \n");
		}
		mysleep (1);
		if (i == 6)
		{
			resume (1);
			printf ("resume thread 1 \n");
		}
	}
}

void fun3 ()
{
	int i = 0;
	for (i = 0; i < 10; i++)
	{
		printf ("thread id=%d time=%d\n", 3, i);
		mysleep (3);
	}
}

int main (void)
{
	int th1, th2, th3;
	thread_create (&th1, fun1);
	thread_create (&th2, fun2);
	thread_create (&th3, fun3);
	detach (th2);
	detach (th3);
	thread_join (th1);
	return 0;
}
