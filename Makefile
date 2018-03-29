CC=clang++
CFLAGS= -std=c++14 -O3 -lm -o

default: query.o

query.o: src/query.cpp src/bktree.cpp src/levenshtein.cpp
	clang++ -std=c++14 -O3 -lm -o query.o src/query.cpp

exploratory.o: exploratory.cpp src/bktree.cpp src/levenshtein.cpp
	clang++ -std=c++14 -O3 -lm -o exploratory.o src/exploratory.cpp

clean:
	-rm -f *.o
