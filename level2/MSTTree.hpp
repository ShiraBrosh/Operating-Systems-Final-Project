#ifndef MSTTREE_HPP
#define MSTTREE_HPP

#include <iostream>
#include <vector>
#include <tuple>

using namespace std;

class MSTTree {
private:
    int vertexCount;  // מספר הצמתים
    vector<tuple<int, int, int>> edges;  // קשתות בפורמט (צומת מקור, צומת יעד, משקל)
    int totalWeight;  // המשקל הכולל של העץ

public:
    MSTTree(int vertexCount);
    void addEdge(int fromVertex, int toVertex, int weight);
    int getTotalWeight() const;
    void printTree() const;
    const vector<tuple<int, int, int>>& getEdges() const;
};

#endif // MSTTREE_HPP
