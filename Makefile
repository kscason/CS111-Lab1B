simpsh: simpsh.o
	gcc -o simpsh simpsh.o

simpsh.o: simpsh.c
	gcc -c -std=c11 -Wall simpsh.c

clean:
	rm simpsh simpsh.o

check:

dist: