/*
线程切换的实现方式:
	现在我们明白了线程切换的两个要点:
		第1个要点是保存现场, 
		第2个要点则是记录接下来需要执行的指令; 

	我们需要细化的考虑一下——现场指的是什么, 接下来需要执行的指令指的是什么, 以及如何保存现场现场该保存在哪里?
	显而易见, "保存现场"则是保存CPU内部的寄存器状态;
	保存接下来需要执行的指令就是需要保存接下来要执行的指令的地址; 
	这些内容该保存到哪里呢?当然是线程的栈里面; 

	具体来说, 线程的切换有以下几个要点:
		我们需要为每一个线程设立一个独立的, 互相不干扰的栈空间; 
		当线程发生切换的时候, 当前线程被切换之前, 需要把自己的现场进行完好的保留, 同时记录下下一条需要执行指令的指令地址; 
		把CPU的栈顶指针寄存器esp切换到即将被调入的线程的堆栈的栈顶地址, 完成了线程栈空间的切换; 
		经过上述这几个步骤, 我们便完成了线程的切换, 由于上面的步骤需要直接访问CPU的寄存器, 于是这个过程往往是采用汇编的方式来进行; 
*/



//
// 线程上下文切换函数switch_to() (弃用, 只能使用汇编版本[仅限x86 架构] 其他cpu 架构学要重写汇编代码)
//


.section .text

//声明switch_to() 函数为全局函数(在这里声明了, 相当于c 语言里面定义了一个普通全局函数) [ps: c语言里面的普通函数都是全局函数]
.global switch_to

//定义全局函数的详细内容
switch_to:
	call closealarm   // 调用函数closealarm()

	push %ebp
	mov %esp, %ebp    // 更改栈帧, 以便寻参

										// 保存现场
	push %edi
	push %esi
	push %ebx
	push %edx
	push %ecx
	push %eax
	pushfl

										// 准备切换栈
	mov current, %eax // 取 current 基址放到 eax
	mov %esp, 8(%eax) // 保存当前 esp 到线程结构体 
	mov 8(%ebp), %eax // 8(%ebp)即为c语言的传入参数next, 取下一个线程结构体基址
	mov %eax, current // 更新 current
	mov 8(%eax), %esp // 切换到下一个线程的栈

										// 恢复现场, 到这里, 已经进入另一个线程环境了, 本质是 esp 改变
	popfl
	popl %eax
	popl %ecx
	popl %edx
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp

	call openalarm    // 调用函数openalarm()

// 汇编式函数返回, 相当于c 语言的return
ret

