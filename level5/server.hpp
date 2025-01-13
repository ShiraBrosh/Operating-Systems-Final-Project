// server.hpp
#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <tuple>
#include <string>
#include <memory>
#include <iostream>
#include "active_object.hpp"
#include "leader_follower.hpp"
#include "mst_metrics.hpp"

class Server {
private:
    std::vector<std::tuple<int, int, int, int>> graph;
    int vertexCount;
    
    // Pipeline components
    std::unique_ptr<ActiveObject<MSTResult>> mst_solver;
    std::unique_ptr<ActiveObject<MSTResult>> metrics_calculator;
    
    // Leader-Follower pool
    std::unique_ptr<LeaderFollowerPool> thread_pool;
    static constexpr size_t THREAD_POOL_SIZE = 4;

    // Helper methods for processing
    MSTResult processMSTPipeline(const std::string& algorithm);
    MSTResult processMSTLeaderFollower(const std::string& algorithm);

public:
    Server();
    void newGraph(int vertexCount, int edgeCount);
    void addEdge(int from, int to, int weight);
    void removeEdge(int from, int to);
    void solveMST(const std::string& algorithm, bool use_pipeline = true);
    void startServer();
    void handleCommand(const std::string& command);
};

#endif // SERVER_HPP