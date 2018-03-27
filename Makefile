CC=clang++
CFLAGS= -std++14 -03

default: a.out

a.out: src/main.cpp src/bktree.cpp src/levenshtein.cpp
	clang++ -v -std=c++14 -O3 src/main.cpp

# a.out: main.o bktree.o levenshtein.o
# 	clang++ -std=c++14 -O3 main.o bktree.o levenshtein.o
#
# main.o: src/main.cpp
# 	clang++ -std=c++14 src/main.cpp
#
# levenshtein.o: src/levenshtein.cpp
# 	clang++ -std=c++14 src/levenshtein.cpp
#
# bktree.o: src/bktree.cpp
# 	clang++ -std=c++14 src/bktree.cpp

# build_wchr: build_wchr.o
# 	clang++ -std=c++14 -O3 -lm -o build_wchr build_wchr.o

# query: query.o
# 	clang++ -std=c++14 -O3 -lm -o query query.o
#
# query_wchr: query_wchr.o
# 	clang++ -std=c++14 -O3 -lm -o query_wchr query_wchr.o

clean:
	-rm -f *.out
	-rn -f *.o
