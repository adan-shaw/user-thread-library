all:
	# all c
	gcc -g3 thread.h thread.c main.c -o main
	gcc -g3 thread.h thread.c test.c -o test

	# asm (for switch_to4x86.s [废弃])
	#gcc -c -m32 switch_to4x86.s -o switch_to.o
	#gcc -c -m32 switch_to4arm.s -o switch_to.o
	#gcc -c -m32 switch_to4mips.s -o switch_to.o
	#gcc -g3 -m32 thread.h thread.c main.c switch_to.o -o main
	#gcc -g3 -m32 thread.h thread.c test.c switch_to.o -o test



clean:
	rm main test
	rm switch_to.o
