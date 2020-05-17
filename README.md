# Socket Programming Project

Author: Kevin Hock Tuen Ng

## Project implementation description

This project was the final project of the course EE450 Introduction to Computer Networks at the University of Southern California. 

The goal was to implement a client-server distributed system for searching for the shortest path. The client's inputs were a mapID, source and destination vertices. There were three majors components in this project: a client, an AWS server, 2 map storage servers and a calculation server.
All the servers and clients used functions to both establish connections and communicate with each other from "ServerFuncs" objects.
The "ServerFuncs" object contained all the necessary TCP and UDP functions in order to enable both reliable and un-reliable communications between clients and servers.
On the other hand, in terms of the map storage servers (i.e. serverA and serverB), both servers were implemented with "WeightedGraph" objects.
The "WeightedGraph" object allowed servers to load maps in .txt format, add or remove edges, search and display mapID.

## Sources files

The following is the list of all the source files in this project:

1. client.cpp
  The client.cpp is the client file for making map queries to server AWS.

2. aws.cpp
  The aws.cpp is one of the backend servers in this project. AWS server will accept requests from clients, and make queries to 2 backend servers with all the map information.

3. serverA.cpp
  The serverA.cpp is the first of the two backend servers in this project that is responsible for one of the map storages, and answer queries from the AWS server.

4. serverB.cpp
  The serverB.cpp is the second of the two backend servers that is responsible for one of the map storages, and also answers queries from the AWS server.

5. serverC.cpp
  The serverC.cpp is the last backend servers in this project. This server is responsible of calculating the shortest path and distance with a given map, source and destination vertices.
  The shortest distance algorithm on this server is the famous Dijkstra's Algorithm. It only answers to queries from the AWS server with the necessary information.

6. ServerFuncs.cpp
  The ServerFuncs.cpp is the source file with all the functions of the ServerFuncs object.
  This source file supports the TCP server, UDP servers, TCP client and UDP client functions.
  The object definition is implemented in the ServerFuncs.h header file.

7. ServerFuncs.h
  The ServerFuncs.h is the header file of ServerFuncs.cpp, with a list of public functions of the object ServerFuncs.

8. WeightedGraph.cpp
  The WeightedGraph.cpp is the source file with longer function definitions of the WeightedGraph object.
  The main responsibility of this object is allowing serverA and serverB to load and store the necessary map information.
  The WeightedGraph object stores map information in a map structure, with mapID as the key and a 2D array as the mapped values.

9. WeightedGraph.h
  The WeightedGraph.h is the header file of WeightedGraph.cpp, with a list of public functions and basic implementation of the object WeightedGraph.

## Reference/Credits

The following is the list of al the resources I have used/referenced in this project:

1. Beej’s Guide to Network Programming Using Internet Sockets, Brian “Beej Jorgensen” Hall

2. GeeksForGeeks Dijkstra's Algorithm: https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/

  This reference was used inside of the ServerFuncs.cpp, where the shortest path algorithm was implemented for serverC.
  The "dij_algo", "updateNewNeighbor" and "findMinDistance" were the functions for making the algorithm possible, and were referenced from reference 1.
  Also, the shortest path algorithm was deployed inside of the "udpServerC" function within the ServerFuncs.cpp file.

3. GeeksForGeeks TCP socket programming: https://www.geeksforgeeks.org/socket-programming-cc

4. Creating a TCP Server in C++[Linux/Code Blocks]: https://www.youtube.com/watch?v=cNdlrbZSkyQ
    https://gist.github.com/codehoose/020c6213f481aee76ea9b096acaddfaf

5. Creating a TCP Client in C++[Linux/Code Blocks]: https://www.youtube.com/watch?v=fmn-pRvNaho
    https://gist.github.com/codehoose/d7dea7010d041d52fb0f59cbe3826036

  References 2, 3 and 4 were used or referenced in the "awsServer" and "tcpClient" functions within the ServerFuncs.cpp files.
  Those references were mainly helpful for implementing the basic structures of TCP communication between clients and servers.
  As the client and AWS server in this project required TCP, the client.cpp and aws.cpp source files both used the "awsServer" and "tcpClient" functions.

6. GeeksForGeeks UDP socket programming: https://www.geeksforgeeks.org/udp-server-client-implementation-c/

7. Starter UDP Server And Client in C++: https://www.youtube.com/watch?v=uIanSvWou1M
    https://bitbucket.org/sloankelly/youtube-source-repository/src/bb84cf7f8d95d37354cf7dd0f0a57e48f393bd4b/cpp/networking/UDPClientServerBasic/UDP_Server/UDP_Server/main.cpp?at=master
    https://bitbucket.org/sloankelly/youtube-source-repository/src/bb84cf7f8d95d37354cf7dd0f0a57e48f393bd4b/cpp/networking/UDPClientServerBasic/UDP_Client/UDP_Client/main.cpp?at=master

  References 5 and 6 were used or referenced in the "udpServer", "udpServerC", "udpClient" and "udpClientToC" functions in ServerFuncs.cpp files.
  Those references were helpful for implementing the basic structures of UDP communication between clients and servers.
  Since the AWS server communicated with serverA, serverB and serverC via UDP, the serverA.cpp, serverB.cpp and serverC.cpp source files used the "udpServer", "udpServerC", "udpClient" and "udpClientToC" functions.

8. Using make and writing Makefiles: https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html

  Reference 7 was referenced during the process of coding the Makefile.

9. GeeksforGeeks Explicitly assigning port number to client in Socket: https://www.geeksforgeeks.org/explicitly-assigning-port-number-client-socket/

  Reference 8 was used or referenced in the "udpClient" and "udpClientToC" functions in ServerFuncs.cpp files.
  That reference was helpful during static port number assignment to AWS for communication between the AWS and the rest of the backend servers.
  As the AWS server would make queries to serverA, serverB and serverC, according to the project description, it required a static port number from the AWS side.
