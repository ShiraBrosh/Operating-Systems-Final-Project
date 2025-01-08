#include "server.hpp"

int main() {
    Server server;
    server.startServer();
    return 0;
}

/*
==========================================
          HOW TO RUN - STAGE 4
==========================================
1. **Compile the Project:**
   Run the following command to compile the server and all required files:
   $ make clean
   $ make

2. **Run the Server:**
   Start the server on port 8080:
   $ ./server_test

3. **Open a Client Connection:**
   Open a second terminal and connect using telnet:
   $ telnet localhost 8080

4. **Send Commands to the Server:**
   - Create a new graph:
     Newgraph <number_of_vertices> <number_of_edges>

   - Add edges to the graph (one per line):
     Addedge <from> <to> <weight>

   - Solve using Prim's algorithm:
     Solve prim

   - Solve using Kruskal's algorithm:
     Solve kruskal

5. **Example Session:**
   Newgraph 5 5
   Addedge 1 2 4
   Addedge 2 3 5
   Addedge 3 4 6
   Addedge 4 5 7
   Addedge 5 1 8
   Solve prim
   Solve kruskal

6. **Expected Output:**
   - If the graph is connected: The MST with its edges will be displayed.
   - If the graph is not connected: An error message will be shown.

==========================================
*/
