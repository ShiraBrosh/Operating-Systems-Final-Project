// server.cpp
#include "server.hpp"
#include "MSTFactory.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

Server::Server() 
    : vertexCount(0)
    , mst_solver(std::make_unique<ActiveObject<MSTResult>>())
    , metrics_calculator(std::make_unique<ActiveObject<MSTResult>>())
    , thread_pool(std::make_unique<LeaderFollowerPool>(THREAD_POOL_SIZE)) {}

void Server::newGraph(int vertexCount, int edgeCount) {
    this->vertexCount = vertexCount;
    graph.clear();
    std::cout << "Created new graph with " << vertexCount << " vertices.\n";
    std::cout << "Expecting " << edgeCount << " edges. Send them using: 'Addedge <from> <to> <weight>'\n";
}

void Server::addEdge(int from, int to, int weight) {
    graph.emplace_back(from, to, weight, graph.size());
    std::cout << "Edge added: " << from << " -> " << to << " (Weight: " << weight << ")\n";
}

void Server::removeEdge(int from, int to) {
    graph.erase(
        std::remove_if(graph.begin(), graph.end(),
            [&](const auto& edge) { 
                return std::get<0>(edge) == from && std::get<1>(edge) == to; 
            }),
        graph.end()
    );
    std::cout << "Edge removed: " << from << " -> " << to << "\n";
}

MSTResult Server::processMSTPipeline(const std::string& algorithm) {
    std::cout << "Pipeline Stage 1: Finding MST...\n";
    auto mst_future = mst_solver->enqueue([this, algorithm]() {
        MSTResult result;
        try {
            std::cout << "  - Starting MST calculation using " << algorithm << "...\n";
            auto mst = MSTFactory::solveMST(graph, vertexCount, algorithm);
            std::vector<std::tuple<int, int, int>> simplified_mst;
            for (const auto& edge : mst) {
                simplified_mst.emplace_back(
                    std::get<0>(edge), 
                    std::get<1>(edge), 
                    std::get<2>(edge)
                );
            }
            result.mst_edges = simplified_mst;
            std::cout << "  - MST calculation completed\n";
        } catch (const std::exception& e) {
            std::cerr << "  - Error in MST calculation: " << e.what() << std::endl;
            throw;
        }
        return result;
    });

    std::cout << "Pipeline Stage 2: Calculating metrics...\n";
    auto metrics_future = metrics_calculator->enqueue([this, mst = std::move(mst_future)]() mutable {
        try {
            std::cout << "  - Starting metrics calculation...\n";
            MSTResult result = mst.get();
            if (result.mst_edges.empty()) {
                throw std::runtime_error("No MST edges found");
            }
            auto final_result = MSTMetrics::calculateMetrics(result.mst_edges, this->vertexCount);
            std::cout << "  - Metrics calculation completed\n";
            return final_result;
        } catch (const std::exception& e) {
            std::cerr << "  - Error in metrics calculation: " << e.what() << std::endl;
            throw;
        }
    });

    return metrics_future.get();
}

MSTResult Server::processMSTLeaderFollower(const std::string& algorithm) {
    std::cout << "Starting Leader-Follower Processing...\n" << std::flush;
    
    // יצירת promise ו-future לקבלת התוצאה
    std::shared_ptr<std::promise<MSTResult>> promise = std::make_shared<std::promise<MSTResult>>();
    std::future<MSTResult> future = promise->get_future();

    // הגשת המשימה לthread pool
    std::cout << "Submitting task to thread pool...\n" << std::flush;
    thread_pool->submit([this, algorithm, promise]() {
        try {
            std::cout << "Worker: Starting MST calculation\n" << std::flush;
            
            // שלב 1: חישוב MST
            auto mst = MSTFactory::solveMST(graph, vertexCount, algorithm);
            std::cout << "Worker: MST calculation completed\n" << std::flush;
            
            // שלב 2: המרת פורמט
            std::vector<std::tuple<int, int, int>> simplified_mst;
            for (const auto& edge : mst) {
                int from = std::get<0>(edge);
                int to = std::get<1>(edge);
                int weight = std::get<2>(edge);
                std::cout << "Worker: Processing edge " << from << "->" << to << " (weight: " << weight << ")\n" << std::flush;
                simplified_mst.emplace_back(from, to, weight);
            }
            
            // שלב 3: חישוב מדדים
            std::cout << "Worker: Starting metrics calculation\n" << std::flush;
            
            // חישוב משקל כולל
            double total_weight = 0;
            for (const auto& edge : simplified_mst) {
                total_weight += std::get<2>(edge);
            }
            
            // יצירת אובייקט התוצאה
            MSTResult result;
            result.mst_edges = simplified_mst;
            result.total_weight = total_weight;
            
            // חישוב מרחקים
            std::vector<std::vector<double>> distances(vertexCount, 
                std::vector<double>(vertexCount, std::numeric_limits<double>::infinity()));
            
            // אתחול מטריצת מרחקים
            for (int i = 0; i < vertexCount; i++) {
                distances[i][i] = 0;
            }
            
            for (const auto& edge : simplified_mst) {
                int from = std::get<0>(edge) - 1;  // התאמה לאינדקס המתחיל מ-0
                int to = std::get<1>(edge) - 1;
                double weight = std::get<2>(edge);
                distances[from][to] = weight;
                distances[to][from] = weight;
            }
            
            // חישוב כל המרחקים באמצעות Floyd-Warshall
            for (int k = 0; k < vertexCount; k++) {
                for (int i = 0; i < vertexCount; i++) {
                    for (int j = 0; j < vertexCount; j++) {
                        if (distances[i][k] != std::numeric_limits<double>::infinity() && 
                            distances[k][j] != std::numeric_limits<double>::infinity()) {
                            distances[i][j] = std::min(distances[i][j], 
                                                     distances[i][k] + distances[k][j]);
                        }
                    }
                }
            }
            
            // חישוב מדדים סופיים
            result.longest_distance = 0;
            result.shortest_distance = std::numeric_limits<double>::infinity();
            double sum_distances = 0;
            int count = 0;
            
            for (int i = 0; i < vertexCount; i++) {
                for (int j = i + 1; j < vertexCount; j++) {
                    if (distances[i][j] != std::numeric_limits<double>::infinity()) {
                        result.longest_distance = std::max(result.longest_distance, distances[i][j]);
                        result.shortest_distance = std::min(result.shortest_distance, distances[i][j]);
                        sum_distances += distances[i][j];
                        count++;
                    }
                }
            }
            
            result.average_distance = count > 0 ? sum_distances / count : 0;
            
            std::cout << "Worker: Metrics calculation completed\n" << std::flush;
            
            // שמירת התוצאה
            promise->set_value(std::move(result));
            std::cout << "Worker: Result set in promise\n" << std::flush;
            
        } catch (const std::exception& e) {
            std::cout << "Worker error: " << e.what() << "\n" << std::flush;
            promise->set_exception(std::current_exception());
        }
    });

    std::cout << "Waiting for task completion...\n" << std::flush;
    
    try {
        // המתנה לתוצאה עם timeout
        auto status = future.wait_for(std::chrono::seconds(30));  // הגדלנו את ה-timeout
        if (status == std::future_status::timeout) {
            throw std::runtime_error("Task timed out after 30 seconds");
        }
        
        // קבלת התוצאה והדפסתה
        MSTResult result = future.get();
        
        std::cout << "\n=== Leader-Follower Results ===\n" << std::flush;
        std::cout << "Total Weight: " << result.total_weight << "\n";
        std::cout << "Longest Distance: " << result.longest_distance << "\n";
        std::cout << "Average Distance: " << result.average_distance << "\n";
        std::cout << "Shortest Distance: " << result.shortest_distance << "\n";
        
        std::cout << "\nMST Edges:\n";
        for (const auto& edge : result.mst_edges) {
            std::cout << std::get<0>(edge) << " -> " << std::get<1>(edge) 
                     << " (Weight: " << std::get<2>(edge) << ")\n";
        }
        std::cout << "===========================\n\n" << std::flush;
        
        return result;
    } catch (const std::exception& e) {
        std::cout << "Error getting results: " << e.what() << "\n" << std::flush;
        throw;
    }

}

void Server::solveMST(const std::string& algorithm, bool use_pipeline) {
    try {
        std::cout << "\n=== Starting MST Calculation ===\n";
        std::cout << "Algorithm: " << algorithm << std::endl;
        std::cout << "Method: " << (use_pipeline ? "Pipeline" : "Leader-Follower") << std::endl;
        std::cout << "============================\n\n";

        MSTResult result;
        if (use_pipeline) {
            std::cout << "Using Pipeline Pattern:\n";
            std::cout << "------------------------\n";
            result = processMSTPipeline(algorithm);
        } else {
            std::cout << "Using Leader-Follower Pattern:\n";
            std::cout << "-----------------------------\n";
            result = processMSTLeaderFollower(algorithm);
        }
        
        // Print detailed report
        std::cout << "\n=== MST Analysis Report ===\n";
        std::cout << "Measurements:\n";
        std::cout << "-------------\n";
        std::cout << "Total MST Weight: " << result.total_weight << "\n";
        std::cout << "Longest Distance: " << result.longest_distance << "\n";
        std::cout << "Average Distance: " << result.average_distance << "\n";
        std::cout << "Shortest Distance: " << result.shortest_distance << "\n";
        
        std::cout << "\nMST Edges:\n";
        std::cout << "----------\n";
        for (const auto& edge : result.mst_edges) {
            std::cout << "Edge " << std::get<0>(edge) << " -> " << std::get<1>(edge) 
                     << " (Weight: " << std::get<2>(edge) << ")\n";
        }
        
        std::cout << "\n=== End of Report ===\n\n";
    } catch (const std::exception& e) {
        std::cerr << "Error processing MST: " << e.what() << std::endl;
    }
}

void Server::handleCommand(const std::string& command) {
    std::stringstream ss(command);
    std::string cmd;
    ss >> cmd;

    try {
        if (cmd == "Newgraph") {
            int n, m;
            ss >> n >> m;
            if (ss.fail()) {
                std::cout << "Invalid format for Newgraph. Usage: Newgraph <vertices> <edges>\n";
                return;
            }
            newGraph(n, m);
        } 
        else if (cmd == "Addedge") {
            int from, to, weight;
            ss >> from >> to >> weight;
            if (ss.fail()) {
                std::cout << "Invalid format for Addedge. Usage: Addedge <from> <to> <weight>\n";
                return;
            }
            addEdge(from, to, weight);
        }
        else if (cmd == "Solve") {
            std::string algo, method;
            ss >> algo >> method;
            
            // הדפסה לצורך דיבוג
            std::cout << "Debug - Received solve command with:\n";
            std::cout << "Algorithm: " << algo << "\n";
            std::cout << "Method: " << method << "\n";

            // בדיקת תקינות הפרמטרים
            if (algo != "kruskal" && algo != "prim") {
                std::cout << "Invalid algorithm. Use 'kruskal' or 'prim'\n";
                return;
            }
            
            if (method != "pipeline" && method != "leader") {
                std::cout << "Invalid method. Use 'pipeline' or 'leader'\n";
                return;
            }

            bool use_pipeline = (method == "pipeline");
            solveMST(algo, use_pipeline);
            
            // הדפסת אישור ביצוע
            std::cout << "Solve command completed.\n";
        }
        else {
            std::cout << "Unknown command: " << cmd << "\n";
            std::cout << "Available commands:\n";
            std::cout << "  Newgraph <vertices> <edges>\n";
            std::cout << "  Addedge <from> <to> <weight>\n";
            std::cout << "  Solve <algorithm> <method>\n";
            std::cout << "    algorithm: kruskal, prim\n";
            std::cout << "    method: pipeline, leader\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error processing command: " << e.what() << "\n";
    }
}


void Server::startServer() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        close(server_fd);
        throw std::runtime_error("Bind failed");
    }

    if (listen(server_fd, 3) < 0) {
        close(server_fd);
        throw std::runtime_error("Listen failed");
    }

    std::cout << "Server is listening on port 8080...\n";

    while (true) {
        int addrlen = sizeof(address);
        int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        std::cout << "Client connected. Ready to receive commands.\n";
        
        char buffer[1024] = {0};
        while (true) {
            int bytesRead = read(new_socket, buffer, 1024);
            if (bytesRead <= 0) {
                std::cout << "Client disconnected.\n";
                break;
            }

            std::string receivedCommand(buffer);
            handleCommand(receivedCommand);
            send(new_socket, "Command received\n", 17, 0);
            memset(buffer, 0, sizeof(buffer));
        }
        
        close(new_socket);
    }
    
    close(server_fd);
}