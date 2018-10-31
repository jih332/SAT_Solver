CC=g++
OPTS=-g -std=c++11 -Werror

all: main.o solver.o clause.o vertex.o
	$(CC) $(OPTS) -lm -o solver main.o solver.o clause.o vertex.o

main.o: solver.h main.cpp
	$(CC) $(OPTS) -c main.cpp

solver.o: solver.h solver.cpp
	$(CC) $(OPTS) -c solver.cpp

clause.o: clause.h clause.cpp
	$(CC) $(OPTS) -c clause.cpp

vertex.o: vertex.h vertex.cpp
	$(CC) $(OPTS) -c vertex.cpp

clean:
	rm -f main.o solver.o clause.o vertex.o predictor;
