#include "server.hpp"
#include "MSTFactory.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

using namespace std;

// בנאי ברירת מחדל
Server::Server() : vertexCount(0) {}

// יצירת גרף חדש
void Server::newGraph(int vertexCount, int edgeCount) {
    this->vertexCount = vertexCount;
    graph.clear();
    cout << "Created new graph with " << vertexCount << " vertices.\n";
    cout << "Expecting " << edgeCount << " edges. Send them using: 'Addedge <from> <to> <weight>'\n";
}

// הוספת קשת
void Server::addEdge(int from, int to, int weight) {
    graph.emplace_back(from, to, weight, graph.size());
    cout << "Edge added: " << from << " -> " << to << " (Weight: " << weight << ")\n";
}

// הסרת קשת
void Server::removeEdge(int from, int to) {
    graph.erase(
        remove_if(graph.begin(), graph.end(), 
                  [&](const auto& edge) { return get<0>(edge) == from && get<1>(edge) == to; }),
        graph.end()
    );
    cout << "Edge removed: " << from << " -> " << to << "\n";
}

// פתרון MST
vector<tuple<int, int, int, int>> Server::solveMST(const string& algorithm) {
    return MSTFactory::solveMST(graph, vertexCount, algorithm);
}

// טיפול בבקשת לקוח
void Server::handleCommand(const string& command) {
    stringstream ss(command);
    string cmd;
    ss >> cmd;

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
        string algo;
        ss >> algo;
        auto result = solveMST(algo);
        cout << "MST Result using " << algo << ":\n";
        for (const auto& edge : result) {
            cout << get<0>(edge) << " -> " << get<1>(edge)
                 << " (Weight: " << get<2>(edge) << ")\n";
        }
    } else {
        cout << "Unknown command received: " << command << endl;
    }
}

// הפעלת שרת עם חיבור מתמשך
void Server::startServer() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);

    cout << "Server is listening on port 8080..." << endl;

    while (true) {  
        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        cout << "Client connected. Ready to receive commands.\n";

        char buffer[1024] = {0};
        while (true) {
            int bytesRead = read(new_socket, buffer, 1024);
            if (bytesRead <= 0) {
                cout << "Client disconnected.\n";
                break;
            }

            string receivedCommand(buffer);
            handleCommand(receivedCommand);
            send(new_socket, "Command received\n", 17, 0);
            memset(buffer, 0, sizeof(buffer));  // ניקוי הבופר
        }

        close(new_socket);
    }

    close(server_fd);
}
