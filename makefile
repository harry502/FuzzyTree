cc = g++
bindir = ./bin
prom = ./bin/FuzzySearch
obj = FuzzySearch.o main.o
DIR = main.cpp
CFG = -std=c++11 -O3

$(prom): $(obj)
	$(cc) -o $(prom) $(obj) $(CFG) -g -lpthread

%.o: %.cpp
	mkdir -p $(bindir) 
	$(cc) $(CFG) -c $< -o $@

clean:
	rm *.o
	rm ../bin/*