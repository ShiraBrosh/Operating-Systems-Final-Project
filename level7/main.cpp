#include "server.hpp"

int main() {
    Server server;
    server.startServer();
    return 0;
}


/*
==========================================
          HOW TO RUN - MST SERVER
==========================================
This guide explains how to compile, run, and test the MST server 
that uses the Leader-Follower pattern and Pipeline pattern 
for solving Minimum Spanning Tree (MST) problems.

📦 Project Structure:
- `server_test`: Main executable for running the server.
- `server.cpp / server.hpp`: Server implementation.
- `kruskal.cpp / prim.cpp`: MST algorithms.
- `leader_follower.cpp / leader_follower.hpp`: Leader-Follower thread pool.
- `active_object.cpp / active_object.hpp`: Active Object implementation.
- `mst_metrics.cpp / mst_metrics.hpp`: MST metrics calculation.
- `Makefile`: Automates compilation and testing.

🛠️ Step 1: Compile the Project
Run the following commands in the terminal:
    $ make clean
    $ make

🖥️ Step 2: Run the Server
Launch the server on port 8080:
    $ ./server_test

🔗 Step 3: Connect a Client (Using Telnet)
Open a second terminal and connect using telnet:
    $ telnet localhost 8080

📡 Step 4: Send Commands to the Server
- Create a new graph:
    Newgraph <number_of_vertices> <number_of_edges>
- Add edges to the graph (one per line):
    Addedge <from> <to> <weight>
- Solve using Prim's algorithm:
    Solve prim pipeline
- Solve using Kruskal's algorithm:
    Solve kruskal leader

📊 Example Session:
1. Newgraph 5 5
2. Addedge 1 2 4
3. Addedge 2 3 5
4. Addedge 3 4 6
5. Addedge 4 5 7
6. Addedge 5 1 8
7. Solve prim pipeline
8. Solve kruskal leader

✅ Expected Output:
- If the graph is connected, the MST will be printed with its edges.
- If the graph is not connected, an error message will be shown.

🧪 Step 5: Run Tests and Valgrind
- Run Valgrind memory check:
    $ make memcheck
- Run Valgrind thread check:
    $ make helgrind

==========================================
*/
