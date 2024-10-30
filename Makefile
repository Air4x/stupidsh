all: stupidsh

stupidsh: main.o
	gcc -o stupidsh src/main.o

main.o:
	gcc -c -ggdb src/main.c -o src/main.o

clean:
	rm -f stupidsh
	rm -f src/main.o
