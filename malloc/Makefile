all:
	gcc -Wall -o test test.c
	gcc -Wall -shared -fpic -o malloc.so malloc.c
run:
	LD_PRELOAD=./malloc.so ./test
