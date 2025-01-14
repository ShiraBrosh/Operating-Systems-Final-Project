#include "prim.hpp"
#include <vector>
#include <queue>
#include <tuple>
#include <iostream>
#include <limits>

using namespace std;

vector<tuple<int, int, int, int>> prim(const vector<tuple<int, int, int, int>>& edges, int n) {
    if (edges.empty() || n == 0) {  
        cerr << "Error: No edges provided or empty graph!\n";
        return {};
    }

    // יצירת רשימת שכנים בגודל n (התאמה לאינדקסים מ-1)
    vector<vector<pair<int, int>>> adj(n);
    for (const auto& edge : edges) {
        int a, b, c, id;
        tie(a, b, c, id) = edge;
        
        // התאמה לאינדקסים מ-1 על ידי הפחתת 1 מכל צומת
        a--; b--;  

        if (a >= n || b >= n || a < 0 || b < 0) {
            cerr << "Error: Node index out of bounds: " << a + 1 << ", " << b + 1 << "\n";
            return {};
        }

        adj[a].emplace_back
(b, c);
        adj[b].emplace_back(a, c);
    }

    cout << "Starting Prim's Algorithm with 1-based indexing support...\n";

    vector<bool> visited(n, false);
    vector<int> minWeight(n, numeric_limits<int>::max());
    vector<int> parent(n, -1);

    // Priority Queue - משקל ראשון, צומת שני
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
    pq.emplace(0, 0);  // התחל מצומת 1 לאחר ההפחתה
    minWeight[0] = 0;

    vector<tuple<int, int, int, int>> mst;
    int edges_added = 0;

    // לולאת האלגוריתם הראשית
    while (!pq.empty() && edges_added < n - 1) {
        int weight, node;
        tie(weight, node) = pq.top();
        pq.pop();

        if (visited[node]) continue;
        visited[node] = true;

        if (parent[node] != -1) {
            mst.emplace_back(parent[node] + 1, node + 1, weight, edges_added);
            edges_added++;
            cout << "MST Edge Added: " << parent[node] + 1 << " -> " << node + 1 << " (Weight: " << weight << ")\n";
        }

        for (const auto& [neighbor, w] : adj[node]) {
            if (!visited[neighbor] && w < minWeight[neighbor]) {
                minWeight[neighbor] = w;
                parent[neighbor] = node;
                pq.emplace(w, neighbor);
            }
        }
    }

    if (edges_added < n - 1) {
        cerr << "Error: Graph is not connected, cannot find MST.\n";
        return {};
    }

    cout << "Prim's Algorithm completed successfully!\n";
    return mst;
}
