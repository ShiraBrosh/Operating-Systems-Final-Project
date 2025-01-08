#include "Graph.hpp"

// Constructor
Graph::Graph(int vertexCount) : vertexCount(vertexCount) {
    adjacencyList.resize(vertexCount);
}

// Add a weighted edge
void Graph::addEdge(int fromVertex, int toVertex, int weight) {
    if (fromVertex < 1 || fromVertex > vertexCount || toVertex < 1 || toVertex > vertexCount) {
        throw invalid_argument("Vertex index out of range.");
    }
    adjacencyList[fromVertex - 1].emplace_back(toVertex - 1, weight);
    adjacencyList[toVertex - 1].emplace_back(fromVertex - 1, weight);  // For undirected graphs
}

// Get the number of vertices
int Graph::getVertexCount() const {
    return vertexCount;
}

// Get the neighbors of a vertex
const vector<pair<int, int>>& Graph::getNeighbors(int vertex) const {
    return adjacencyList[vertex - 1];
}

// Print the graph for debugging
void Graph::printGraph() const {
    for (int i = 0; i < vertexCount; ++i) {
        cout << "Vertex " << (i + 1) << ":";
        for (const auto& [neighbor, weight] : adjacencyList[i]) {
            cout << " -> " << (neighbor + 1) << " (weight " << weight << ")";
        }
        cout << endl;
    }
}
