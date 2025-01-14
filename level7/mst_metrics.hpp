// mst_metrics.hpp
#ifndef MST_METRICS_HPP
#define MST_METRICS_HPP

#include <vector>
#include <tuple>
#include <limits>
#include <stdexcept>
#include <iostream>

struct MSTResult {
    std::vector<std::tuple<int, int, int>> mst_edges;
    double total_weight;
    double longest_distance;
    double average_distance;
    double shortest_distance;
    
    // Constructor with default values
    MSTResult() : 
        total_weight(0),
        longest_distance(0),
        average_distance(0),
        shortest_distance(std::numeric_limits<double>::infinity()) {}
};

class MSTMetrics {
public:
    // Calculate all metrics for a given MST
    static MSTResult calculateMetrics(const std::vector<std::tuple<int, int, int>>& mst_edges, int vertex_count);
    
    // Individual metric calculations
    static double calculateTotalWeight(const std::vector<std::tuple<int, int, int>>& mst_edges);
    static double calculateLongestDistance(const std::vector<std::tuple<int, int, int>>& mst_edges, int vertex_count);
    static double calculateAverageDistance(const std::vector<std::tuple<int, int, int>>& mst_edges, int vertex_count);
    static double calculateShortestDistance(const std::vector<std::tuple<int, int, int>>& mst_edges);

private:
    static std::vector<std::vector<double>> buildDistanceMatrix(
        const std::vector<std::tuple<int, int, int>>& mst_edges, 
        int vertex_count
    );
};

#endif // MST_METRICS_HPP