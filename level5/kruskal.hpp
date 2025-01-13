// kruskal.hpp
#ifndef KRUSKAL_H
#define KRUSKAL_H

#include <utility>
#include <vector>
#include <tuple>

using namespace std;

// Kruskal's algorithm for Minimum Spanning Tree with connectivity check
vector<tuple<int, int, int, int>> kruskal(const vector<tuple<int, int, int, int>>& graph_edges, int n);

#endif
