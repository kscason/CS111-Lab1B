simpsh: simpsh.o
	gcc -o simpsh simpsh.o

simpsh.o: simpsh.c
	gcc -c -std=c11 -Wall simpsh.c

clean:
	rm -rf simpsh simpsh.o lab1-adamjones.tar.gz

check: simpsh
	./test.sh

dist: check
	tar -cvf lab1-adamjones.tar.gz Makefile README simpsh.c test.sh
	