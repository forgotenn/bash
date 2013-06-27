all:
	gcc -Wall -o test test.c
	g++ -Wall -shared -fpic -o malloc.so malloc.cpp
run:
	LD_PRELOAD=./malloc.so ./test
vim:
	LD_PRELOAD=./malloc.so vim
myself:
	LD_PRELOAD=./malloc.so g++ -Wall -o malloc malloc.cpp
