#g++ -g inner_function.cpp -o Python -I/usr/include/python2.7 -L/usr/lib/python2.7 -lpython2.7 -lboost_serialization -std=c++11 
#executable file
PROGROM=

CC=g++

CFLAGS=-g

SRC=$(wildcard *.c)

OBJ=$(SRC:.c=.o)

all:depend $(PROGROM)
depend:
	@(CC) -MM (SRC) > .depend

-include .depend
$(PROGROM):(OBJ)
	$(CC) $(CFLAGS)

clean:
	@rm (PROGROM) (OBJ) .depend -f

