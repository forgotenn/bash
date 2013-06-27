all:
	gcc -Wall -o test test.c
	gcc -Wall -shared -fpic -o malloc.so malloc.cpp
run:
	LD_PRELOAD=./malloc.so ./test
vim:
	LD_PRELOAD=./malloc.so vim
