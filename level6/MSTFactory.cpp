#include "MSTFactory.hpp"
#include "kruskal.hpp"
#include "prim.hpp"
#include <iostream>

using namespace std;

// Factory method for solving the MST using chosen algorithm
vector<tuple<int, int, int, int>> MSTFactory::solveMST(
    const vector<tuple<int, int, int, int>>& edges, 
    int vertexCount, 
    const string& algorithm) 
{
    if (algorithm == "kruskal") {
        cout << "Solving MST using Kruskal's algorithm..." << endl;
        return kruskal(edges, vertexCount);
    } 
    else if (algorithm == "prim") {
        cout << "Solving MST using Prim's algorithm..." << endl;
        return prim(edges, vertexCount);
    } 
    else {
        cerr << "Error: Unknown algorithm requested: " << algorithm << endl;
        return {};
    }
}
