CC=clang++
CFLAGS= -std=c++14 -O3 -lm -o

default: a.out

# a.out: src/main.cpp src/bktree.cpp src/levenshtein.cpp
# 	clang++ -v -std=c++14 -O3 src/main.cpp

a.out: main.o bktree.o levenshtein.o
	clang++ -std=c++14 -O3 -lm -o main.o bktree.o levenshtein.o

main: main.o
	clang++ -std=c++14 -O3 -lm -o main main.o

levenshtein: levenshtein.o
	clang++ -std=c++14 -O3 -lm -o levenshtein levenshtin.o

bktree: bktree.o
	clang++ -std=c++14 -O3 -lm -o bktree bktre.o

query: query.o
	clang++ -std=c++14 -O3 -lm -o query query.o

# build_wchr: build_wchr.o
# 	clang++ -std=c++14 -O3 -lm -o build_wchr build_wchr.o
#
# query: query.o
# 	clang++ -std=c++14 -O3 -lm -o query query.o
#
# query_wchr: query_wchr.o
# 	clang++ -std=c++14 -O3 -lm -o query_wchr query_wchr.o

clean:
	-rn -f *.o
