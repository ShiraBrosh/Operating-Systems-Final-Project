#ifndef MST_FACTORY_HPP
#define MST_FACTORY_HPP

#include <vector>
#include <tuple>
#include <string>

using namespace std;

class MSTFactory {
public:
    // Factory method to solve the MST using the chosen algorithm
    static vector<tuple<int, int, int, int>> solveMST(
        const vector<tuple<int, int, int, int>>& edges, 
        int vertexCount, 
        const string& algorithm);
};

#endif // MST_FACTORY_HPP

