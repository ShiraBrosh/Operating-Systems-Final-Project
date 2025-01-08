#include "MSTTree.hpp"

// Constructor
MSTTree::MSTTree(int vertexCount) : vertexCount(vertexCount), totalWeight(0) {}

// Add an edge to the MST
void MSTTree::addEdge(int fromVertex, int toVertex, int weight) {
    edges.emplace_back(fromVertex, toVertex, weight);
    totalWeight += weight;
}

// Get the total weight of the MST
int MSTTree::getTotalWeight() const {
    return totalWeight;
}

// Print the MST for debugging
void MSTTree::printTree() const {
    cout << "MST Tree with " << vertexCount << " vertices:\n";
    for (const auto& [from, to, weight] : edges) {
        cout << "Edge: " << from << " -> " << to << " (Weight: " << weight << ")\n";
    }
    cout << "Total weight: " << totalWeight << endl;
}

// Get all edges in the MST
const vector<tuple<int, int, int>>& MSTTree::getEdges() const {
    return edges;
}
