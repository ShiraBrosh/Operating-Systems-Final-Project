CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

all: mst_tree_test

mst_tree_test: MSTTree.o mst_tree_test.o
	$(CXX) $(CXXFLAGS) MSTTree.o mst_tree_test.o -o mst_tree_test

MSTTree.o: MSTTree.cpp MSTTree.hpp
	$(CXX) $(CXXFLAGS) -c MSTTree.cpp

mst_tree_test.o: mst_tree_test.cpp MSTTree.hpp
	$(CXX) $(CXXFLAGS) -c mst_tree_test.cpp

clean:
	rm -f *.o mst_tree_test
