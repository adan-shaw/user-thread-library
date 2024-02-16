all:
	# all c
	#gcc -g3 thread.h thread.c main.c -o main
	#gcc -g3 thread.h thread.c test.c -o test
	# asm
	gcc -c switch_to.s -o switch_to.o
	gcc -g3 thread.h thread.c main.c switch_to.o -o main
	gcc -g3 thread.h thread.c test.c switch_to.o -o test



clean:
	rm main test
	rm switch_to.o
