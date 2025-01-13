// kruskal.cpp
#include "kruskal.hpp"
#include "union_find.hpp"
#include <algorithm>
#include <iostream>
#include <tuple>
#include <vector>

using namespace std;

vector<tuple<int, int, int, int>> kruskal(const vector<tuple<int, int, int, int>>& graph_edges, int n) {
    UnionFind graph(n);  // מבנה נתונים לבדיקת רכיבי קשירות
    vector<tuple<int, int, int, int>> edges = graph_edges;
    vector<tuple<int, int, int, int>> spanning_tree;

    // מיון הקשתות לפי משקל
    sort(edges.begin(), edges.end(), [](const auto& a, const auto& b) {
        return get<2>(a) < get<2>(b);
    });

    int edges_added = 0;  // סופר כמה קשתות נוספו ל-MST

    // מעבר על הקשתות ובדיקת האיחוד
    for (const auto& edge : edges) {
        int from, to, cost, id;
        tie(from, to, cost, id) = edge;

        // הורדת 1 מהאינדקסים (התאמה לאינדקסים שמתחילים מ-1)
        from--;
        to--;

        if (graph.unite(from, to)) {  
            spanning_tree.emplace_back(from + 1, to + 1, cost, id);
            edges_added++;
        }
    }

    // בדיקה אם הגרף קשיר
    if (edges_added < n - 1) {
        cerr << "Error: Graph is not connected, cannot find MST using Kruskal.\n";
        return {};  // החזרת רשימה ריקה אם הגרף אינו קשיר
    }

    cout << "Kruskal's Algorithm completed successfully! MST found.\n";
    return spanning_tree;
}
