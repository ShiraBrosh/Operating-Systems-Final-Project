// mst_metrics.cpp
#include "mst_metrics.hpp"
#include <algorithm>
#include <queue>
#include <set>

// בקובץ mst_metrics.cpp

// mst_metrics.cpp
MSTResult MSTMetrics::calculateMetrics(
    const std::vector<std::tuple<int, int, int>>& mst_edges, 
    int vertex_count) 
{
    std::cout << "Starting metrics calculation...\n";
    MSTResult result;
    result.mst_edges = mst_edges;

    try {
        // 1. חישוב משקל כולל
        std::cout << "Calculating total weight...\n";
        result.total_weight = 0;
        for (const auto& edge : mst_edges) {
            result.total_weight += std::get<2>(edge);
        }
        std::cout << "Total weight calculated: " << result.total_weight << "\n";

        // 2. בניית מטריצת מרחקים
        std::cout << "Building distance matrix...\n";
        auto distances = std::vector<std::vector<double>>(
            vertex_count, 
            std::vector<double>(vertex_count, std::numeric_limits<double>::infinity())
        );

        // אתחול המטריצה
        for (int i = 0; i < vertex_count; i++) {
            distances[i][i] = 0;  // המרחק מצומת לעצמו הוא 0
        }

        // הוספת הקשתות למטריצה
        for (const auto& edge : mst_edges) {
            int from = std::get<0>(edge) - 1;  // מתאים לאינדקס המתחיל מ-0
            int to = std::get<1>(edge) - 1;
            double weight = std::get<2>(edge);
            distances[from][to] = weight;
            distances[to][from] = weight;  // גרף לא מכוון
        }

        // Floyd-Warshall לחישוב כל המרחקים הקצרים
        std::cout << "Computing all shortest paths...\n";
        for (int k = 0; k < vertex_count; k++) {
            for (int i = 0; i < vertex_count; i++) {
                for (int j = 0; j < vertex_count; j++) {
                    if (distances[i][k] != std::numeric_limits<double>::infinity() &&
                        distances[k][j] != std::numeric_limits<double>::infinity()) {
                        distances[i][j] = std::min(
                            distances[i][j], 
                            distances[i][k] + distances[k][j]
                        );
                    }
                }
            }
        }

        // 3. חישוב המדדים
        std::cout << "Computing final metrics...\n";
        result.longest_distance = 0;
        result.shortest_distance = std::numeric_limits<double>::infinity();
        double sum_distances = 0;
        int count = 0;

        for (int i = 0; i < vertex_count; i++) {
            for (int j = i + 1; j < vertex_count; j++) {
                if (distances[i][j] != std::numeric_limits<double>::infinity()) {
                    result.longest_distance = std::max(result.longest_distance, distances[i][j]);
                    result.shortest_distance = std::min(result.shortest_distance, distances[i][j]);
                    sum_distances += distances[i][j];
                    count++;
                }
            }
        }

        result.average_distance = count > 0 ? sum_distances / count : 0;
        
        std::cout << "Metrics calculation completed!\n";
        std::cout << "Results:\n";
        std::cout << "- Total Weight: " << result.total_weight << "\n";
        std::cout << "- Longest Distance: " << result.longest_distance << "\n";
        std::cout << "- Shortest Distance: " << result.shortest_distance << "\n";
        std::cout << "- Average Distance: " << result.average_distance << "\n";

        return result;

    } catch (const std::exception& e) {
        std::cerr << "Error calculating metrics: " << e.what() << "\n";
        throw;
    }
}

double MSTMetrics::calculateTotalWeight(
    const std::vector<std::tuple<int, int, int>>& mst_edges) 
{
    double total = 0;
    for (const auto& edge : mst_edges) {
        total += std::get<2>(edge); // הוספת המשקל של כל קשת
    }
    return total;
}

std::vector<std::vector<double>> MSTMetrics::buildDistanceMatrix(
    const std::vector<std::tuple<int, int, int>>& mst_edges,
    int vertex_count)
{
    // Initialize distance matrix with infinity
    std::vector<std::vector<double>> dist(
        vertex_count,
        std::vector<double>(vertex_count, std::numeric_limits<double>::infinity())
    );
    
    // Set diagonal to 0
    for (int i = 0; i < vertex_count; i++) {
        dist[i][i] = 0;
    }
    
    // Add edges to matrix
    for (const auto& edge : mst_edges) {
        int from = std::get<0>(edge) - 1; // התאמה לאינדקס המתחיל מ-0
        int to = std::get<1>(edge) - 1;
        double weight = std::get<2>(edge);
        dist[from][to] = weight;
        dist[to][from] = weight; // גרף לא מכוון
    }
    
    // Floyd-Warshall algorithm
    for (int k = 0; k < vertex_count; k++) {
        for (int i = 0; i < vertex_count; i++) {
            for (int j = 0; j < vertex_count; j++) {
                if (dist[i][k] != std::numeric_limits<double>::infinity() &&
                    dist[k][j] != std::numeric_limits<double>::infinity()) {
                    dist[i][j] = std::min(dist[i][j], dist[i][k] + dist[k][j]);
                }
            }
        }
    }
    
    return dist;
}

double MSTMetrics::calculateLongestDistance(
    const std::vector<std::tuple<int, int, int>>& mst_edges,
    int vertex_count)
{
    auto dist = buildDistanceMatrix(mst_edges, vertex_count);
    double max_distance = 0;
    
    for (int i = 0; i < vertex_count; i++) {
        for (int j = i + 1; j < vertex_count; j++) {
            if (dist[i][j] != std::numeric_limits<double>::infinity()) {
                max_distance = std::max(max_distance, dist[i][j]);
            }
        }
    }
    
    return max_distance;
}

double MSTMetrics::calculateAverageDistance(
    const std::vector<std::tuple<int, int, int>>& mst_edges,
    int vertex_count)
{
    auto dist = buildDistanceMatrix(mst_edges, vertex_count);
    double sum = 0;
    int count = 0;
    
    for (int i = 0; i < vertex_count; i++) {
        for (int j = i + 1; j < vertex_count; j++) {
            if (dist[i][j] != std::numeric_limits<double>::infinity()) {
                sum += dist[i][j];
                count++;
            }
        }
    }
    
    return count > 0 ? sum / count : 0;
}

double MSTMetrics::calculateShortestDistance(
    const std::vector<std::tuple<int, int, int>>& mst_edges)
{
    if (mst_edges.empty()) {
        return std::numeric_limits<double>::infinity();
    }
    
    double min_distance = std::numeric_limits<double>::infinity();
    for (const auto& edge : mst_edges) {
        min_distance = std::min(min_distance, static_cast<double>(std::get<2>(edge)));
    }
    
    return min_distance;
}