default: build

%.o: %.cpp
	clang++ -std=c++14

build: main.o
	clang++ -std=c++14 -O3 -lm -o main main.o

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
	-rm -f build query build_wchr quiry_wchr
