#include "MSTFactory.hpp"
#include <iostream>

using namespace std;

int main() {
    // יצירת רשימת קשתות לדוגמה עם צמתים, משקלים ומזהה קשת
    vector<tuple<int, int, int, int>> edges = {
        {0, 1, 4, 1},
        {0, 2, 3, 2},
        {1, 2, 1, 3},
        {1, 3, 2, 4},
        {2, 3, 5, 5},
        {3, 4, 1, 6}
    };

    int vertexCount = 5;  // מספר הצמתים בגרף

    // בדיקת האלגוריתם Kruskal
    cout << "\n=== Running Kruskal's Algorithm ===\n";
    auto mstKruskal = MSTFactory::solveMST(edges, vertexCount, "kruskal");
    cout << "Edges in the MST (Kruskal):\n";
    for (const auto& edge : mstKruskal) {
        cout << "Edge: " << get<0>(edge) << " -> " << get<1>(edge)
             << ", Weight: " << get<2>(edge) << endl;
    }

    // בדיקת האלגוריתם Prim
    cout << "\n=== Running Prim's Algorithm ===\n";
    auto mstPrim = MSTFactory::solveMST(edges, vertexCount, "prim");
    cout << "Edges in the MST (Prim):\n";
    for (const auto& edge : mstPrim) {
        cout << "Edge: " << get<0>(edge) << " -> " << get<1>(edge)
             << ", Weight: " << get<2>(edge) << endl;
    }

    return 0;
}
