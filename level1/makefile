# Makefile for MyGraph's Algorithm

CXX = g++
CXXFLAGS = -Wall -g -I.

TARGET = mygraph
OBJS = main.o Graph.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp Graph.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

Graph.o: Graph.cpp Graph.hpp
	$(CXX) $(CXXFLAGS) -c Graph.cpp

clean:
	rm -f $(OBJS) $(TARGET)
