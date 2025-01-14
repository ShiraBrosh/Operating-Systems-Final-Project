// union_find.cpp
#include "union_find.hpp"
#include <numeric>

UnionFind::UnionFind(int _n) : n(_n), parent(_n), rank(_n, 1) {
    iota(parent.begin(), parent.end(), 0);
}


int UnionFind::find_parent(int node) {
    if (parent[node] != node)
        parent[node] = find_parent(parent[node]);  // דחיסת נתיב
    return parent[node];
}

bool UnionFind::unite(int x, int y) {
    x = find_parent(x);
    y = find_parent(y);
    if (x == y) return false;
    if (rank[x] < rank[y]) {
        parent[x] = y;
    } else {
        parent[y] = x;
        if (rank[x] == rank[y]) rank[x]++;
    }
    return true;
}
