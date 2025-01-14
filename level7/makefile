CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -fprofile-arcs -ftest-coverage
LDFLAGS = -pthread -lgcov

# Target executable
TARGET = server_test

# Source files
SOURCES = main.cpp \
          server.cpp \
          kruskal.cpp \
          prim.cpp \
          union_find.cpp \
          MSTFactory.cpp \
          active_object.cpp \
          leader_follower.cpp \
          mst_metrics.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Header files
HEADERS = server.hpp \
          kruskal.hpp \
          prim.hpp \
          union_find.hpp \
          MSTFactory.hpp \
          active_object.hpp \
          leader_follower.hpp \
          mst_metrics.hpp

# Default target
all: $(TARGET)

# Linking the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compiling source files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJECTS) $(TARGET) *.gcno *.gcda

# Valgrind memory check
memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Valgrind thread check
helgrind: $(TARGET)
	valgrind --tool=helgrind ./$(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean memcheck helgrind run