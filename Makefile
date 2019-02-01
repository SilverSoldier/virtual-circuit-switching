main: main.cpp main.h init.o
	g++ -Wall -g main.cpp main.h init.h init.o -o main

init: init.cpp init.h
	g++ -Wall -g init.cpp init.h -c
