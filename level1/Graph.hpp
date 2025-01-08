#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <iostream>
#include <vector>
#include <limits>
#include <tuple>
#include <algorithm>
#include <stdexcept>

using namespace std;

class Graph {
private:
    int vertexCount;  // Number of vertices in the graph
    vector<vector<pair<int, int>>> adjacencyList; // (target, weight)

public:
    Graph(int vertexCount);
    void addEdge(int fromVertex, int toVertex, int weight);
    int getVertexCount() const;
    const vector<pair<int, int>>& getNeighbors(int vertex) const;
    void printGraph() const;
};

#endif // GRAPH_HPP
