CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

all: server_test

server_test: server.o MSTFactory.o kruskal.o prim.o union_find.o main.o
	$(CXX) $(CXXFLAGS) server.o MSTFactory.o kruskal.o prim.o union_find.o main.o -o server_test

server.o: server.cpp server.hpp
	$(CXX) $(CXXFLAGS) -c server.cpp

MSTFactory.o: MSTFactory.cpp MSTFactory.hpp
	$(CXX) $(CXXFLAGS) -c MSTFactory.cpp

kruskal.o: kruskal.cpp kruskal.hpp
	$(CXX) $(CXXFLAGS) -c kruskal.cpp

prim.o: prim.cpp prim.hpp
	$(CXX) $(CXXFLAGS) -c prim.cpp

union_find.o: union_find.cpp union_find.hpp
	$(CXX) $(CXXFLAGS) -c union_find.cpp

main.o: main.cpp server.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

clean:
	rm -f *.o server_test
