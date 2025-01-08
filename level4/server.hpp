#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <tuple>
#include <string>

using namespace std;

class Server {
private:
    vector<tuple<int, int, int, int>> graph;
    int vertexCount;
public:
    Server();
    void newGraph(int vertexCount, int edgeCount);
    void addEdge(int from, int to, int weight);
    void removeEdge(int from, int to);
    vector<tuple<int, int, int, int>> solveMST(const string& algorithm);
    void startServer();
    void handleCommand(const string& command);
};

#endif // SERVER_HPP
