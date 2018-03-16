default: a.out

a.out: src/main.cpp src/bktree.cpp
	clang++ -std=c++14 -O3 src/main.cpp

# build_wchr: build_wchr.o
# 	clang++ -std=c++14 -O3 -lm -o build_wchr build_wchr.o

# query: query.o
# 	clang++ -std=c++14 -O3 -lm -o query query.o
#
# query_wchr: query_wchr.o
# 	clang++ -std=c++14 -O3 -lm -o query_wchr query_wchr.o

clean:
	-rm -f *.o
	-rm -f *.d
	-rm -f build query
