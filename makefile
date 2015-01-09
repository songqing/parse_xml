CC = gcc
CFLAGS = -g -Wall -lpthread -O3 
main:main.o step1.o stack.o step2.o step3.o
	$(CC) main.o step1.o stack.o step2.o step3.o -O3 -o main
main.o:main.c
	$(CC) $(CFLAGS) -c main.c -o main.o
step1.o:step1.c
	$(CC) $(CFLAGS) -c step1.c -o step1.o
stack.o:stack.c
	$(CC) $(CFLAGS) -c stack.c -o stack.o
step2.o:step2.c
	$(CC) $(CFLAGS) -c step2.c -o step2.o
step3.o:step3.c
	$(CC) $(CFLAGS) -c step3.c -o step3.o
clean:
	rm -rf *.o
