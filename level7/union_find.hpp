#ifndef UNION_FIND_H
#define UNION_FIND_H

#include <vector>     // לשימוש ב-vector
#include <numeric>    // לשימוש ב-iota
using namespace std;

class UnionFind {
private:
    vector<int> parent, rank; // parent מוכרז לפני rank
    int n;  // מספר הצמתים
public:
    explicit UnionFind(int n);   // קונסטרקטור
    int find_parent(int node);   // מציאת שורש הקבוצה
    bool unite(int x, int y);    // איחוד בין קבוצות
};

#endif // UNION_FIND_H
