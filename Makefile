all:
	gcc -g -c sha2.c
	gcc -g -c main.c
	gcc main.o sha2.o -o randstorm

clean:
	rm -f *.o
	rm -f randstorm

