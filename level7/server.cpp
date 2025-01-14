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
   // Stage 1: Find MST
   auto mst_future = mst_solver->enqueue([this, algorithm]() {
       MSTResult result;
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
       return result;
   });

   // Stage 2: Calculate metrics
   return metrics_calculator->enqueue([this, mst = std::move(mst_future)]() mutable {
       MSTResult result = mst.get();
       if (result.mst_edges.empty()) {
           throw std::runtime_error("No MST edges found");
       }
       return MSTMetrics::calculateMetrics(result.mst_edges, this->vertexCount);
   }).get();
}

MSTResult Server::processMSTLeaderFollower(const std::string& algorithm) {
   std::shared_ptr<std::promise<MSTResult>> promise = std::make_shared<std::promise<MSTResult>>();
   std::future<MSTResult> future = promise->get_future();

   thread_pool->submit([this, algorithm, promise]() {
       try {
           auto mst = MSTFactory::solveMST(graph, vertexCount, algorithm);
           std::vector<std::tuple<int, int, int>> simplified_mst;
           for (const auto& edge : mst) {
               simplified_mst.emplace_back(
                   std::get<0>(edge), 
                   std::get<1>(edge), 
                   std::get<2>(edge)
               );
           }
           auto result = MSTMetrics::calculateMetrics(simplified_mst, vertexCount);
           promise->set_value(std::move(result));
           
       } catch (const std::exception& e) {
           promise->set_exception(std::current_exception());
       }
   });

   auto status = future.wait_for(std::chrono::seconds(30));
   if (status == std::future_status::timeout) {
       throw std::runtime_error("Computation timed out");
   }
   
   return future.get();
}

void Server::solveMST(const std::string& algorithm, bool use_pipeline) {
   try {
       std::cout << "\n=== MST Calculation Report ===\n";
       std::cout << "Algorithm: " << algorithm << "\n";
       std::cout << "Processing Method: " << (use_pipeline ? "Pipeline" : "Leader-Follower") << "\n";
       std::cout << "============================\n\n";

       MSTResult result;
       if (use_pipeline) {
           result = processMSTPipeline(algorithm);
       } else {
           result = processMSTLeaderFollower(algorithm);
       }
       
       std::cout << "Measurements:\n";
       std::cout << "-------------\n";
       std::cout << "Total MST Weight: " << result.total_weight << "\n";
       std::cout << "Longest Distance: " << result.longest_distance << "\n";
       std::cout << "Average Distance: " << result.average_distance << "\n";
       std::cout << "Shortest Distance: " << result.shortest_distance << "\n\n";
       
       std::cout << "MST Edges:\n";
       std::cout << "----------\n";
       for (const auto& edge : result.mst_edges) {
           std::cout << "Edge " << std::get<0>(edge) << " -> " << std::get<1>(edge) 
                    << " (Weight: " << std::get<2>(edge) << ")\n";
       }
       std::cout << "\n=== End of Report ===\n\n";

   } catch (const std::exception& e) {
       std::cerr << "Error: " << e.what() << std::endl;
   }
}

bool Server::handleCommand(const std::string& command) {
   std::stringstream ss(command);
   std::string cmd;
   ss >> cmd;

   if (cmd=="QUIT"){
       return true; // חיווי לסגירת השרת
   }

   if (cmd == "Newgraph") {
       int n, m;
       ss >> n >> m;
       newGraph(n, m);
   } else if (cmd == "Addedge") {
       int from, to, weight;
       ss >> from >> to >> weight;
       addEdge(from, to, weight);
   } else if (cmd == "Removeedge") {
       int from, to;
       ss >> from >> to;
       removeEdge(from, to);
   } else if (cmd == "Solve") {
       std::string algo, method;
       ss >> algo >> method;
       bool use_pipeline = (method != "leader");
       solveMST(algo, use_pipeline);
   } else {
       std::cout << "Unknown command: " << command << std::endl;
   }

    return false; // המשך פעילות השרת

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
           bool shouldQuit = handleCommand(receivedCommand);
           send(new_socket, "Command received\n", 17, 0);
           memset(buffer, 0, sizeof(buffer));

           if (shouldQuit) {
               std::cout << "Shutting down server...\n";
               close(new_socket);
               close(server_fd);
               return; // סגור את השרת
           }
       }
       
       close(new_socket);
   }
   
   close(server_fd);
}