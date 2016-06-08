LIB=-lpython2.7 -lboost_serialization -lpthread
STD=-std=c++11

CC=g++
FLAGS=-g -w

PROGRAM=test test1 test2

#ifeq($(PROGRAM), $(wildcard(test)))
#	rm $(PROGRAM)
#endif

all:$(PROGRAM)
test:test.cpp
	$(CC) $(FLAGS) $< -o $@ $(LIB) $(STD)

test1:test1.cpp
	$(CC) $(FLAGS) $< -o $@ $(LIB) $(STD)

test2:test2.cpp
	$(CC) $(FLAGS) $< -o $@ $(LIB) $(STD)

.PHONY:
	clean

clean:
	rm test test1 test2
