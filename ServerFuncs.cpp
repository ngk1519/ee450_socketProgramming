#include <iostream>
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <map>
#include <algorithm>
#include <math.h>
#include "ServerFuncs.h"
//#include "WeightedGraph.h"

using namespace std;

#define QUEUESIZE 256
#define MAXNUM 100
#define INF_NUM 200000

// TCP server constants
const int bufferSize = 1024;
// const int hostBufferSize = 1000;
// const int serviceBufferSize = 32;

// UDP server constants
const int clientInfoSize = 256;

// IP address for the local machine
const string serverIP = "127.0.0.1";

char* maxNumString = "100";

// For compiling this file in linux, it requires a flag "std=c++0x" in order to
// recognize the to_string function

// Find the next shortest path that has not been visited
// Reference from: https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/
int findMinDistance(float distance[], bool visited[], int size){
  int min = INF_NUM, index = 0;
  if (sizeof(distance) > 0 && sizeof(visited) > 0){
    for (int i = 0; i < size; i++){
      if (!visited[i] && distance[i] <= min){
        min = distance[i];
        index = i;
      }
    }
  }
  return index;
}

// Update all the new neighbors' shortest distance
// Reference from: https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/
void updateNewNeighbor(float** overallMap, float distance[], bool visited[], int previous[], int source, int size){
  if (sizeof(overallMap) > 0 && sizeof(distance) > 0 && sizeof(visited) > 0){
    for (int k = 0; k < size; k++){
      // Update the shortest distance except the original source
      if (!visited[k] && overallMap[source][k] > 0 && distance[source] != INF_NUM &&
         (overallMap[source][k]+distance[source]) < distance[k]) {
        distance[k] = overallMap[source][k]+distance[source];
        previous[k] = source;
        //cout << "Distance changed from " << source << " to " << k << " to value " << distance[k] << "\n";
      }
    }
  }
}

// Return the shortest path of a given source and destination in string format
string trackShortestPath (int source, int dest, int previous[]){
  string result = to_string(dest).append(" -- ");
  int currentIndex = dest;
  while (previous[currentIndex] != -1){
    if (previous[currentIndex] == source) result.append(to_string(previous[currentIndex]));
    else result.append(to_string(previous[currentIndex])).append(" -- ");
    currentIndex = previous[currentIndex];
  }
  // reverse the result string;
  reverse(result.begin(), result.end());
  int stringLen = strlen(result.c_str()), counter = 0;
  while (counter < stringLen-1){
    if (isdigit(result[counter]) && isdigit(result[counter+1])) swap(result[counter],result[counter+1]);
    counter++;
  }
  return result;
}

// The dijkstra's algorithm
// Reference from: https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/
void dij_algo(float** overallMap, int source, int dest, float* shortestDis, string* shortestPath, int mapSize){
  // Initialization
  float* distFromSource = new float[mapSize];
  bool* visitedNode = new bool[mapSize];
  int* previousNode = new int[mapSize];
  int nextMin = 0;

  // Look for all neighbors from the given source
  // If not neighbors, distance sets to infinity
  for (int i = 0; i < mapSize; i++){
    distFromSource[i] = INF_NUM;
    visitedNode[i] = false;
  }

  // The source node has 0 distance from itself
  distFromSource[source] = 0;
  previousNode[source] = -1;

  // Iterate through the map by looking for the next shortest path
  for (int j = 0; j < mapSize-1; j++){
    nextMin = findMinDistance(distFromSource,visitedNode,mapSize);
    visitedNode[nextMin] = true;
    updateNewNeighbor(overallMap,distFromSource,visitedNode,previousNode,nextMin,mapSize);
  }

  // track down the actual shortest path from given source to given destination
  *shortestPath = trackShortestPath(source,dest,previousNode);
  *shortestDis = distFromSource[dest];
}

// A function that determines whether source vertex exist or not in map
bool checkSource(float* vertexMap, int vertexMapSize){
  int counter = 0;
  bool exist = false;
  while (counter < vertexMapSize && !exist){
    if (vertexMap[counter] > 0) exist = true;
    counter++;
  }
  return exist;
}

// A function that determines whether dest vertex exist or not in map
bool checkDest(float** wholeMap, int mapSize, int dest){
  int counter = 0;
  bool exist = false;
  while (counter < mapSize){
    if (wholeMap[counter][dest] > 0) exist = true;
    counter++;
  }
  return exist;
}

// Display edges from the given source to all others ndoes
void ServerFuncs::displayRow(float* inputArray, int size){
  cout << "[";
  for (int j = 0; j < size; j++){
    if (j == size - 1) cout << inputArray[j];
    else cout << inputArray[j] << " ";
  }
  cout << "]\n";
}

/*
  Implementation of TCP client

  Credit:
  Beej’s Guide to Network Programming Using Internet Sockets, Brian “Beej Jorgensen” Hall
  GeeksForGeeks TCP socket programming: https://www.geeksforgeeks.org/socket-programming-cc
  Creating a TCP Server in C++[Linux/Code Blocks]: https://www.youtube.com/watch?v=cNdlrbZSkyQ
  https://gist.github.com/codehoose/020c6213f481aee76ea9b096acaddfaf
*/
void ServerFuncs::tcpClient(string serverIPAddr, int port, int inputSize, char* input[]){
  // Creat a socket
  int clientSocket, connection;
  int sendMess = 0, receiveMess = 0;
  // socket(Domain = Internet access, type = stream (aka TCP), protocol = 0 (default))
  if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    cout << "Client cannot create a socket \n";
  }

  // Connect to the server with the socket
  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(port);
  // Convert the string of serverIP to binary
  inet_pton(AF_INET, serverIPAddr.c_str(), &serverAddress.sin_addr);


  if ((connection = connect(clientSocket, (const struct sockaddr*)&serverAddress, sizeof(serverAddress))) == -1) {
    cout << "Client failed to connect with server \n";
  } else  cout << "The client is up and running\n";

  // Interaction with the server
  char buffer[bufferSize];
  // Send user input to server
  for (int i = 1; i < inputSize; i++){
    sendMess = send(clientSocket, input[i], sizeof(input[i]), 0);
    if (sendMess == -1) cerr << "Unable to send it to server\n";
    memset(buffer, 0, bufferSize);
    receiveMess = recv(clientSocket, buffer, bufferSize, 0);
    if (receiveMess == -1) cerr << "Unable to receive info from server\n";
  }
  cout << "The client has sent a query to AWS using TCP: starting vertex " << input[2] <<
      "; destination vertex " << input[3] << "; map ID " << input[1] << "; file size " <<
      input[4] << "\n";

  // Receive a message from AWS to anticipate one of the following responses:
  // 0. No matching mapID
  // 1. No matching source vertex
  // 2. No matching destination vertex
  // 3. Neither the source nor the destination vertex matches
  // 4. All went fine, prepare to receive calculated results from AWS
  receiveMess = recv(clientSocket, buffer, bufferSize, 0);
  if (receiveMess == -1) cerr << "Unable to receive info from server\n";

  char noMapID[bufferSize] = "No Map ";
  strcat(noMapID,input[1]);
  char noSource[bufferSize] = "No vertex ";
  strcat(noSource,input[2]);
  char noDest[bufferSize] = "No vertex ";
  strcat(noDest,input[3]);
  //char neither[bufferSize] = "Neither vertex";
  // Case 0: no matching mapID
  if (strcmp(buffer,noMapID) == 0) cout << "No map ID " << input[1] << " found\n";
  // Case 1: no matching source vertex
  else if (strcmp(buffer,noSource) == 0) cout << "No source vertex " << input[2] << " found\n";
  // Case 2: no matching destination vertex
  else if (strcmp(buffer,noDest) == 0) cout << "No dest vertex " << input[3] << " found\n";
  // Case 3: Neither the source nor the destination vertex matches
  else if (strcmp(buffer,"Neither vertex") == 0) cout << "No source " << input[2] << " nor dest " << input[3] << " vertex found\n";
  // Case 4: All fine
  else if (strcmp(buffer,"Map Info") == 0){
    cout << "Client received " << buffer << " from AWS\n";
    // start receiving calcuated map info from aws
    char** calculatedMapResult = new char*[4];
    char* tempBuffer = new char[bufferSize];
    sendMess = send(clientSocket, tempBuffer, bufferSize, 0);
    if (sendMess == -1) cerr << "Unable to send it to server\n";
    for (int i = 0; i < 4; i++){
      calculatedMapResult[i] = new char[bufferSize];
      receiveMess = recv(clientSocket, calculatedMapResult[i], bufferSize, 0);
      if (receiveMess == -1) cerr << "Unable to receive info from server\n";
      else cout << "Received message: " << calculatedMapResult[i] << "\n";
      sendMess = send(clientSocket, tempBuffer, bufferSize, 0);
      if (sendMess == -1) cerr << "Unable to send it to server\n";
    }
    // The total distance
    float minLength = atof(calculatedMapResult[0]);
    // Calculate the total delay
    float propagationDelay = atof(calculatedMapResult[2]);
    float transmissionDelay = atof(calculatedMapResult[3]);
    float totalDelay = transmissionDelay + propagationDelay;

    //Print out the calculated results
    cout << "The client has received results from AWS: \n";
    cout << "--------------------------------------------------------\n";
    cout << "Source    Destination    Min Length   Tt    Tp    Delay \n";
    cout << input[2] << "          " << input[3] << "            ";
         // << calculatedMapResult[0] << "     " << calculatedMapResult[3] << "     " << calculatedMapResult[2]<< "     " << totalDelay << "\n";
    printf("%.2f   %.2f     %.2f    %.2f",minLength,transmissionDelay,propagationDelay,totalDelay);
    cout << "\n-------------------------------------------------------\n";
    cout << "Shortest path: " << calculatedMapResult[1] << "\n";
  }

  // Close the socket
  close(clientSocket);
}

/*
  Implementation of UDP client

  Credit:
  Beej’s Guide to Network Programming Using Internet Sockets, Brian “Beej Jorgensen” Hall
  GeeksForGeeks UDP socket programming: https://www.geeksforgeeks.org/udp-server-client-implementation-c/
  Starter UDP Server And Client in C++: https://www.youtube.com/watch?v=uIanSvWou1M
  https://bitbucket.org/sloankelly/youtube-source-repository/src/bb84cf7f8d95d37354cf7dd0f0a57e48f393bd4b/cpp/networking/UDPClientServerBasic/UDP_Server/UDP_Server/main.cpp?at=master
  https://bitbucket.org/sloankelly/youtube-source-repository/src/bb84cf7f8d95d37354cf7dd0f0a57e48f393bd4b/cpp/networking/UDPClientServerBasic/UDP_Client/UDP_Client/main.cpp?at=master
  GeeksforGeeks Explicitly assigning port number to client in Socket: https://www.geeksforgeeks.org/explicitly-assigning-port-number-client-socket/
*/
void ServerFuncs::udpClient(string serverIPAddr, int port, int port2, int inputSize, char* input[], bool* found, float**& resultMap, char*& prop, char*& trans, string serverName){
  // Implementation of a UDP client
  char* buffer = new char[MAXNUM];
  int sendMessage = 0, receiveMessage = 0;
  // Create a socket on the client side
  int socketClient = socket(AF_INET,SOCK_DGRAM,0);
  if (socketClient == -1){
    cerr <<  "Client unable to create a socket\n";
  } //else cout << "UDP client successfully created a socket\n";
  struct sockaddr_in socketDescriptor;
  memset(&socketDescriptor,0,sizeof(socketDescriptor));
  inet_pton(AF_INET, serverIPAddr.c_str(),&socketDescriptor.sin_addr.s_addr);
  socketDescriptor.sin_family = AF_INET;
  socketDescriptor.sin_port = htons(port);
  int socketSize = sizeof(socketDescriptor);

  // Bind the static and given port number to the socket descriptor
  struct sockaddr_in socketDescriptor2;
  memset(&socketDescriptor2,0,sizeof(socketDescriptor2));
  inet_pton(AF_INET, serverIP.c_str(),&socketDescriptor2.sin_addr.s_addr);
  socketDescriptor2.sin_family = AF_INET;
  socketDescriptor2.sin_port = htons(port2);
  if (bind(socketClient,(const struct sockaddr*)&socketDescriptor2, sizeof(socketDescriptor2)) == -1){
    cerr << " unables to create a socket descriptor\n";
  } //else cout << "up and running using UDP on port " << port2 << "\n";

  struct sockaddr_in check;
  int check_len = sizeof(check);
  if (getsockname(socketClient, (struct sockaddr*)&check, (socklen_t *)&check_len) == -1) {
      cerr << "Unable to get socket's port \n";
   } // else cout << "Socket's port number: " << ntohs(check.sin_port) << "\n";

   cout << "The AWS has sent mapID to server" << serverName << " using UDP over port " << ntohs(check.sin_port) << "\n";

  // send mapID to current server
  sendMessage = sendto(socketClient,input[0],sizeof(input[0]),0,(struct sockaddr*)&socketDescriptor,socketSize);
  if (sendMessage == -1) cerr << "Unable to send input message \n";
  else cout << "Sent the mapID " << input[0] << " to server\n";

  //memset(buffer, 0, bufferSize);
  receiveMessage = recvfrom(socketClient,buffer,bufferSize,0,(struct sockaddr*)&socketDescriptor,(socklen_t*)&socketSize);
  if (receiveMessage == -1) cerr << "Unable to receive message \n";
  else {
    //cout << "Received the message \n";
    if (strcmp(buffer,"Graph Found") == 0){
      // Send the source vertex to server
      *found = true;
      // sendMessage = sendto(socketClient,input[1],sizeof(input[1]),0,(struct sockaddr*)&socketDescriptor,socketSize);
      // if (sendMessage == -1) cerr << "Unable to send input message \n";
      // else cout << "Sent the source vertex " << input[1] << " to server\n";

      // A char* 2D array for storing a map
      int i = 0, j = 0, charBuffer = 16;
      char* recvMapString[MAXNUM][MAXNUM];
      for (i = 0; i < MAXNUM; i++) {
        //recvMapString[i] = malloc(MAXNUM*sizeof(char**));
        for (j = 0; j < MAXNUM; j++) recvMapString[i][j] = new char[charBuffer];
      }

      // Receive the actual map from one of the map storage server by each element
      for (i = 0; i < MAXNUM; i++){
        for (j = 0; j < MAXNUM; j++){
          receiveMessage = recvfrom(socketClient,recvMapString[i][j],charBuffer,0,(struct sockaddr*)&socketDescriptor,(socklen_t*)&socketSize);
          if (receiveMessage == -1) cerr << "Unables to receive source map\n";
          //else cout << "Keep receiving from server: " << recvMapString[i][j] << "\n";

          char* tempBuffer = new char[charBuffer];
          sendMessage = sendto(socketClient,tempBuffer,charBuffer,0,(struct sockaddr*)&socketDescriptor,socketSize);
          if (sendMessage == -1) cerr << "Unable to send input message \n";
        }
      }
      // Conversion from received char* 2D array to float 2D array
      for (i = 0; i < MAXNUM; i++) {
        for (j = 0; j < MAXNUM; j++) resultMap[i][j] = strtof(recvMapString[i][j],NULL);
      }

      // Receive propagation delay from the map storage server
      char* propBuffer = new char[charBuffer];
      receiveMessage = recvfrom(socketClient,propBuffer,charBuffer,0,(struct sockaddr*)&socketDescriptor,(socklen_t*)&socketSize);
      if (receiveMessage == -1) cerr << "Unables to receive source map\n";
      //cout << "Received prop from server: " << propBuffer << "\n";
      char* tempBuffer = new char[charBuffer];
      sendMessage = sendto(socketClient,tempBuffer,charBuffer,0,(struct sockaddr*)&socketDescriptor,socketSize);
      if (sendMessage == -1) cerr << "Unable to send input message \n";
      prop = propBuffer;

      // Received transmission delay from the map storage server
      char* transBuffer = new char[charBuffer];
      receiveMessage = recvfrom(socketClient,transBuffer,charBuffer,0,(struct sockaddr*)&socketDescriptor,(socklen_t*)&socketSize);
      if (receiveMessage == -1) cerr << "Unables to receive source map\n";
      //cout << "Received trans from server: " << transBuffer << "\n";
      sendMessage = sendto(socketClient,tempBuffer,charBuffer,0,(struct sockaddr*)&socketDescriptor,socketSize);
      if (sendMessage == -1) cerr << "Unable to send input message \n";
      trans = transBuffer;

    } else if (strcmp(buffer,"Graph not Found") == 0){
      cout << "No map ID " << input[0] << " found in server " << serverName << "\n";
    }
  }
  // close socket
  close(socketClient);
}

/*
  Implementation of UDP client to serverC
  MapInfo array order: prop, trans, mapID, source, dest

  Credit:
  Beej’s Guide to Network Programming Using Internet Sockets, Brian “Beej Jorgensen” Hall
  GeeksForGeeks UDP socket programming: https://www.geeksforgeeks.org/udp-server-client-implementation-c/
  Starter UDP Server And Client in C++: https://www.youtube.com/watch?v=uIanSvWou1M
  https://bitbucket.org/sloankelly/youtube-source-repository/src/bb84cf7f8d95d37354cf7dd0f0a57e48f393bd4b/cpp/networking/UDPClientServerBasic/UDP_Server/UDP_Server/main.cpp?at=master
  https://bitbucket.org/sloankelly/youtube-source-repository/src/bb84cf7f8d95d37354cf7dd0f0a57e48f393bd4b/cpp/networking/UDPClientServerBasic/UDP_Client/UDP_Client/main.cpp?at=master
  GeeksforGeeks Explicitly assigning port number to client in Socket: https://www.geeksforgeeks.org/explicitly-assigning-port-number-client-socket/
*/
void ServerFuncs::udpClientToC(string serverIPAddr, int port, int port2, char* mapInfo[], float** resultMap, char**& calculatedResult){
  // Implementation of a UDP client
  int charBuffer = 1024;
  char* buffer = new char[charBuffer];
  // Create a socket on the client side
  int socketClient = socket(AF_INET,SOCK_DGRAM,0);
  if (socketClient == -1){
    cerr <<  "Client unable to create a socket\n";
  } //else cout << "UDP client successfully created a socket\n";

  struct sockaddr_in socketDescriptor;
  memset(&socketDescriptor,0,sizeof(socketDescriptor));
  inet_pton(AF_INET, serverIPAddr.c_str(),&socketDescriptor.sin_addr.s_addr);
  socketDescriptor.sin_family = AF_INET;
  socketDescriptor.sin_port = htons(port);
  int socketSize = sizeof(socketDescriptor);

  // Bind the static and given port number to the socket descriptor
  struct sockaddr_in socketDescriptor2;
  memset(&socketDescriptor2,0,sizeof(socketDescriptor2));
  inet_pton(AF_INET, serverIP.c_str(),&socketDescriptor2.sin_addr.s_addr);
  socketDescriptor2.sin_family = AF_INET;
  socketDescriptor2.sin_port = htons(port2);
  if (bind(socketClient,(const struct sockaddr*)&socketDescriptor2, sizeof(socketDescriptor2)) == -1){
    cerr << " unables to create a socket descriptor\n";
  } //else cout << "up and running using UDP on port " << port2 << "\n";

  struct sockaddr_in sa;
  int sa_len = sizeof(sa);
  if (getsockname(socketClient, (struct sockaddr*)&sa, (socklen_t *)&sa_len) == -1) {
      cerr << "Unable to get socket's port \n";
   } // else cout << "Socket's port number: " << ntohs(sa.sin_port) << "\n";

  int sent = 0, received = 0, i = 0, j = 0, mapInfoSize = 7, calculatedInfoSize = 4;

  // Send mapInfo
  for (i = 0; i < mapInfoSize; i++){
    sent = sendto(socketClient,mapInfo[i],sizeof(mapInfo[i]),0,(struct sockaddr*)&socketDescriptor,socketSize);
    if (sent == -1) cerr << "Unable to send input message \n";
    //else cout << "Sent " << mapInfo[i] << " to serverC\n";

    //memset(buffer, 0, charBuffer);
    received = recvfrom(socketClient,buffer,charBuffer,0,(struct sockaddr*)&socketDescriptor,(socklen_t*)&socketSize);
    if (received == -1) cerr << "Unable to receive message from server\n";
  }
  cout << "The AWS has sent map, source ID, destination ID, propagration speed and transmission speed to server C using UDP over port " <<
        ntohs(sa.sin_port) << " \n";

  // Convert float-typed map to char* typed map
  int mapSize = atoi(mapInfo[5]);
  // cout << "The mapSize: " << mapSize << "\n";
  char* stringMap[mapSize][mapSize];
  for (i = 0; i < mapSize; i++) {
    for (j = 0; j < mapSize; j++) {
      stringMap[i][j] = new char[charBuffer];
      int n = sprintf(stringMap[i][j],"%f",resultMap[i][j]);
    }
  }
  // Send the actual result map in char* form
  for (i = 0; i < mapSize; i++){
    for (j = 0; j < mapSize; j++){
      sent = sendto(socketClient,stringMap[i][j],sizeof(resultMap[i][j]),0,(struct sockaddr*)&socketDescriptor,socketSize);
      if (sent == -1) cerr << "Unable to send input message \n";
      //else cout << "Sent " << resultMap[i][j] << " to server\n";

      //memset(buffer, 0, charBuffer);
      received = recvfrom(socketClient,buffer,charBuffer,0,(struct sockaddr*)&socketDescriptor,(socklen_t*)&socketSize);
      if (received == -1) cerr << "Unable to receive message from server\n";
    }
  }

  // Receive the calculated map info from serverC in char* format
  char** recvCalculatedInfo = new char*[calculatedInfoSize];
  for (i = 0; i < calculatedInfoSize; i++){
    recvCalculatedInfo[i] = new char[charBuffer];
    received = recvfrom(socketClient,recvCalculatedInfo[i],charBuffer,0,(struct sockaddr*)&socketDescriptor,(socklen_t*)&socketSize);
    if (received == -1) cerr << "Unable to receive message from serverC \n";
    //else cout << "Received " << recvCalculatedInfo[i] << " from serverC\n";
    calculatedResult[i] = recvCalculatedInfo[i];

    //memset(buffer, 0, charBuffer);
    sent = sendto(socketClient,buffer,charBuffer,0,(struct sockaddr*)&socketDescriptor,socketSize);
    if (sent == -1) cerr << "Unable to send input message \n";
  }
  // cout << "The AWS has received results from server C: \n";
  // cout << "The shortest path: " << recvCalculatedInfo[1] << "\n";
  // cout << "The shortest distance: " << recvCalculatedInfo[0] << " km\n";
  // cout << "Transmission delay: " << recvCalculatedInfo[3] << " sec \n";
  // cout << "Propagation delay: " << recvCalculatedInfo[2] << " sec\n";

  close(socketClient);
}

/*
  Implementation of TCP server

  Credit:
  Beej’s Guide to Network Programming Using Internet Sockets, Brian “Beej Jorgensen” Hall
  GeeksForGeeks TCP socket programming: https://www.geeksforgeeks.org/socket-programming-cc
  Creating a TCP Server in C++[Linux/Code Blocks]: https://www.youtube.com/watch?v=cNdlrbZSkyQ
  https://gist.github.com/codehoose/020c6213f481aee76ea9b096acaddfaf
*/
void ServerFuncs::awsServer(string serverIPAddr, int port, int portA, int portB, int portC, int port2, float** resultMap, string serverName){
  int parentSocket, childSocket;
  int charBuffer = 1024;
  int sent = 0, received = 0;
  char buffer[bufferSize] = {0};
  char** clientInfo = new char*[4];
  char s[1024] = {0};

  // create a "parent" socket
  if ((parentSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    cerr << "Server cannot create a socket" << '\n';
  }
  // Bind IP and port in an old fashion
  struct sockaddr_in serverHints;
  memset(&serverHints,0,sizeof(serverHints));
  serverHints.sin_family = AF_INET;
  serverHints.sin_port = htons(port);
  inet_pton(AF_INET, serverIPAddr.c_str(), &serverHints.sin_addr);
  if (bind(parentSocket, (const struct sockaddr*)&serverHints, sizeof(serverHints)) == -1){
    cerr << "Server cannot bind IP and port" << "\n";
  }
  // Listen
  if (listen(parentSocket, QUEUESIZE) == -1){
    cerr << "Server cannot listen" << "\n";
  } else cout << "The AWS is up and running\n";

  // Keep the TCP server on till Ctrl+C
  while(true){
    // Accept calls from client and create a "child" socket
    struct sockaddr_in child;
    int childLength = sizeof(child);
    if ((childSocket = accept(parentSocket, (struct sockaddr*)&child, (socklen_t*)&childLength)) == -1){
      cerr << "Server unable to accept clients" << "\n";
    } //else cout << "Server accepted a new client with port " << ntohs(child.sin_port) << "\n";

    // close socket
    // close(parentSocket);

    // Display the incoming message from the client
    for (int i = 0; i < 4; i++){
      memset(buffer, 0, bufferSize);
      int receive = recv(childSocket, buffer, bufferSize, 0);
      char* result = new char[bufferSize];
      strcpy(result, buffer);
      clientInfo[i] = result;
      if (receive == -1){
        cerr << "Unable to receive\n";
        break;
      }
      if (receive == 0){
        cout << "The client has disconnected \n";
        break;
      }
      sent = send(childSocket, buffer, receive+1, 0);
      if (sent == -1) cerr << "Unable to send\n";
    }

    //close(childSocket);
    cout << "The AWS has received map ID " << clientInfo[0] << ", start vertex "<< clientInfo[1] <<
    " destination vertex " << clientInfo[2] << " and file size " << clientInfo[3] <<
    " from the client using TCP over port " << ntohs(child.sin_port) << "\n";

    // Sent necessary info to serverA and check with serverA
    //float** tempMap = new float*[MAXNUM];
    bool foundGraph = false, foundGraphB = false;
    bool foundSource = false, foundDest = false;
    float** tempMap = new float*[MAXNUM];
    for (int i = 0; i < MAXNUM; i++) tempMap[i] = new float[bufferSize];
    int s = 0, d = 0, source = 0, dest = 0;
    char* propagation = new char[charBuffer];
    char* transmission = new char[charBuffer];
    sscanf(clientInfo[1],"%d",&s);
    source = s;
    sscanf(clientInfo[2],"%d",&d);
    dest = d;
    // Request mapInfo from serverA via UDP
    udpClient(serverIPAddr,portA,port2,sizeof(clientInfo),clientInfo,&foundGraph,tempMap,propagation,transmission,"A");
    if (foundGraph){
      // After receiving the desired map, check source and destination vertex
      cout << "The AWS has received map information from serverA \n";
      foundSource = checkSource(tempMap[source],MAXNUM);
      foundDest = checkDest(tempMap,MAXNUM,dest);
      if (foundSource && foundDest) {
        cout << "Propagation speed: " << propagation << "\n";
        cout << "Transmission speed: " << transmission << "\n";
      }
      //displayRow(tempMap[source], MAXNUM);
    } else {
      // If serverA does not have the mapID, request mapInfo from serverB via UDP
      // Also after receiving the desired map, check source and destination vertex
      udpClient(serverIPAddr,portB,port2,sizeof(clientInfo),clientInfo,&foundGraphB,tempMap,propagation,transmission,"B");
      if (foundGraphB){
        cout << "The AWS has received map information from serverB \n";
        foundSource = checkSource(tempMap[source],MAXNUM);
        foundDest = checkDest(tempMap,MAXNUM,dest);
        if (foundSource && foundDest) {
          cout << "Propagation speed: " << propagation << "\n";
          cout << "Transmission speed: " << transmission << "\n";
        }
        //displayRow(tempMap[source], MAXNUM);
      }
      // else {
      //   // Cannot find the given mapID
      //   // cout << "Input mapID not found in the graph, sending error to client using TCP over port " << "\n";
      //   char errorMess[charBuffer];
      //   strcpy(errorMess,"No Map ");
      //   strcat(errorMess,clientInfo[0]);
      //   sent = send(childSocket, errorMess, charBuffer, 0);
      //   if (sent == -1) cerr << "Server " << serverName << " unables to send no map message to client\n";
      //   close(childSocket);
      // }
    }

    if (foundGraph || foundGraphB){
      // establish connection with serverC if either foundGraph == true or foundGraphB == true
      char errorMess[charBuffer];
      if (foundSource && foundDest){
        cout << "The source and destination vertex are in the graph\n";
        char** mapInfo = new char*[7];
        mapInfo[0] = propagation; // Propagration speed
        mapInfo[1] = transmission; // transmission speed
        mapInfo[2] = clientInfo[0]; // mapID
        mapInfo[3] = clientInfo[1]; // source
        mapInfo[4] = clientInfo[2]; // destination
        mapInfo[5] = maxNumString; // mapSize
        mapInfo[6] = clientInfo[3]; // fileSize
        char** calculatedResult = new char*[4];
        udpClientToC(serverIPAddr,portC,port2,mapInfo,tempMap,calculatedResult);
        cout << "The AWS has received results from serverC: \n";
        cout << "Shortest path: " << calculatedResult[1] << " \n";
        //cout << "Size of shortestPath: " << strlen(calculatedResult[1]) << "\n";
        cout << "Shortest distance: " << calculatedResult[0] << " km \n";
        //cout << "Size of shortestDis: " << strlen(calculatedResult[0]) << "\n";
        cout << "Transmission delay: " << calculatedResult[3] << " sec \n";
        cout << "Propagation delay: " << calculatedResult[2] << " sec \n";

        //Send the necessary calculated map result to client
        char* mess = "Map Info";
        sent = send(childSocket, mess, charBuffer, 0);
        if (sent == -1) cerr << "Server " << serverName << " unables to send calculated map info to client\n";
        received = recv(childSocket, buffer, bufferSize, 0);
        if (received == -1) cerr << "Server " << serverName << " unables to received from client\n";
        for (int i = 0; i < 4; i++){
          sent = send(childSocket, calculatedResult[i], charBuffer, 0);
          if (sent == -1) cerr << "Server " << serverName << " unables to send calculated map info to client\n";
          //else cout << "Server " << serverName << " sent " << calculatedResult[i] << "\n";
          received = recv(childSocket, buffer, bufferSize, 0);
          if (received == -1) cerr << "Server " << serverName << " unables to received from client\n";
        }
        cout << "The AWS has sent calculated results to client using TCP over port " << ntohs(child.sin_port) << "\n";
      } else if (!foundSource) {
        cout << "Source vertex " << source << " not found in the graph, sending error to client using TCP over port " << ntohs(child.sin_port) << "\n";
        strcpy(errorMess,"No vertex ");
        strcat(errorMess,clientInfo[1]);
      } else if (!foundDest) {
        cout << "Destination vertex " << dest << " not found in the graph, sending error to client using TCP over port " << ntohs(child.sin_port) << "\n";
        strcpy(errorMess,"No vertex ");
        strcat(errorMess,clientInfo[2]);
      } else {
        cout << "Both the source and destination vertex not found in the graph, sending error to client using TCP over port " << ntohs(child.sin_port) << "\n";
        strcpy(errorMess,"Neither vertex");
      }
      send(childSocket, errorMess, charBuffer, 0);
    } else {
      // No matching mapID from neither serverA nor serverB
      char errorMess[charBuffer];
      strcpy(errorMess,"No Map ");
      strcat(errorMess,clientInfo[0]);
      sent = send(childSocket, errorMess, charBuffer, 0);
      if (sent == -1) cerr << "Server " << serverName << " unables to send no map message to client\n";
      // close(childSocket);
    }
    //else {
      // char errorMess[charBuffer];
      // if (!foundSource) {
      //   cout << "Source vertex " << source << " not found in the graph, sending error to client using TCP over port " << ntohs(child.sin_port) << "\n";
      //   strcpy(errorMess,"No vertex ");
      //   strcat(errorMess,clientInfo[1]);
      // }
      // else if (!foundDest) {
      //   cout << "Destination vertex " << dest << " not found in the graph, sending error to client using TCP over port " << ntohs(child.sin_port) << "\n";
      //   strcpy(errorMess,"No vertex ");
      //   strcat(errorMess,clientInfo[2]);
      // }
      // else {
      //   cout << "Both the source and destination vertex not found in the graph, sending error to client using TCP over port " << ntohs(child.sin_port) << "\n";
      //   strcpy(errorMess,"Neither vertex");
      // }
      // send(childSocket, errorMess, charBuffer, 0);
    //}
    close(childSocket);
  }
}

/*
  Implementation of UDP server

  Credit:
  Beej’s Guide to Network Programming Using Internet Sockets, Brian “Beej Jorgensen” Hall
  GeeksForGeeks UDP socket programming: https://www.geeksforgeeks.org/udp-server-client-implementation-c/
  Starter UDP Server And Client in C++: https://www.youtube.com/watch?v=uIanSvWou1M
  https://bitbucket.org/sloankelly/youtube-source-repository/src/bb84cf7f8d95d37354cf7dd0f0a57e48f393bd4b/cpp/networking/UDPClientServerBasic/UDP_Server/UDP_Server/main.cpp?at=master
  https://bitbucket.org/sloankelly/youtube-source-repository/src/bb84cf7f8d95d37354cf7dd0f0a57e48f393bd4b/cpp/networking/UDPClientServerBasic/UDP_Client/UDP_Client/main.cpp?at=master
*/
void ServerFuncs::udpServer(string serverIPAddr, int port, string serverName, WeightedGraph maps){
  // Creation of a socket
  char* buffer= new char[bufferSize];
  int socketUDP = socket(AF_INET,SOCK_DGRAM,0);
  if (socketUDP == -1){
    cerr << "Server" << serverName << " unables to create a socket\n";
  } else cout << "Server " << serverName << " successfully created a socket\n";

  // Bind the IP address and the port number for the socket descriptor
  struct sockaddr_in socketDescriptor;
  memset(&socketDescriptor,0,sizeof(socketDescriptor));
  inet_pton(AF_INET, serverIP.c_str(),&socketDescriptor.sin_addr.s_addr);
  socketDescriptor.sin_family = AF_INET;
  socketDescriptor.sin_port = htons(port);
  if (bind(socketUDP,(const struct sockaddr*)&socketDescriptor, sizeof(socketDescriptor)) == -1){
    cerr << "Server" << serverName << " unables to create a socket descriptor\n";
  } else cout << "Server " << serverName << " is up and running using UDP on port " << ntohs(socketDescriptor.sin_port) << "\n";

  // struct sockaddr_in sin;
  // socklen_t len = sizeof(sin);
  // if (getsockname(socketUDP, (struct sockaddr *)&sin, &len) == -1) perror("getsockname");
  // else cout << serverName << " port number " << ntohs(sin.sin_port) << "\n";

  // Receive any client
  struct sockaddr_in client;

  while (true){
    memset(&client,0,sizeof(client));
    int clientLength = sizeof(client);
    int received = 0, sent = 0, source = 0;
    char* foundGraph = "Graph not Found";
    char* result = new char[bufferSize];
    memset(buffer, 0, bufferSize);

    // Receive both the mapID and source vertex from AWS
    received = recvfrom(socketUDP,buffer,sizeof(buffer),0,(struct sockaddr*)&client,(socklen_t*)&clientLength);
    strcpy(result, buffer);
    if (received == -1) cerr << "Error in receiving bytes from client. \n";
    // else cout << "Server received " << result << " from a client\n";

    char clientInfo[clientInfoSize];
    memset(clientInfo, 0, clientInfoSize);
    inet_ntop(AF_INET,&client.sin_addr,clientInfo,clientInfoSize);
    // cout << "Client port " << client.sin_port << " message: " << result << "\n";
    cout << "The server " << serverName << " has received input for finding graph of map " << result << "\n";

    // Determine whether current server contains the input mapID or not
    if (!maps.containsMap(result)){
      cout << "The server " << serverName << " does not have the required graph ID " << result << "\n";
      sent = sendto(socketUDP,foundGraph,strlen(foundGraph),0,(struct sockaddr*)&client,clientLength);
      if (sent == -1) cerr << "The Server " << serverName << " unables to reply AWS\n";
      else cout << "The Server " << serverName << " has send " << foundGraph << " to AWS\n";
    } else {
      // Send "Graph Found" so that the server can receive the source vertex
      foundGraph = "Graph Found";
      char* tempID = result;
      sent = sendto(socketUDP,foundGraph,strlen(foundGraph),0,(struct sockaddr*)&client,clientLength);
      if (sent == -1) cerr << "The server " << serverName << " unables to reply AWS with found answers\n";
      else cout << "The Server " << serverName <<  " has sent " << foundGraph << " to AWS\n";
      memset(buffer, 0, bufferSize);

      // Receive source vertex from AWS
      // received = recvfrom(socketUDP,buffer,sizeof(buffer),0,(struct sockaddr*)&client,(socklen_t*)&clientLength);
      // if (received == -1) cerr << "Error in receiving bytes from client. \n";
      // else cout << "Server received " << buffer << " from a client\n";

      // get the necessary information from about the map
      //sscanf(buffer,"%d",&source);
      float** tempMap = maps.getMap(result);
      float propagation = maps.getProp(result);
      int transmission = maps.getTransmission(result);
      //displayRow(tempMap[source], MAXNUM);

      // Send the overall map of the given mapID
      int i = 0, j = 0, charBuffer = 256;
      int n = 0;
      // 2D string array for sending mapInfo
      // Converting 2D float-typed array to 2D string array
      char* totalString[MAXNUM][MAXNUM];
      for (i = 0; i < MAXNUM; i++) {
        for (j = 0; j < MAXNUM; j++){
          totalString[i][j] = new char[charBuffer];
          n = sprintf(totalString[i][j],"%f",tempMap[i][j]);
        }
      }
      for (i = 0; i < MAXNUM; i++) {
        for (j = 0; j < MAXNUM; j++){
          sent = sendto(socketUDP,totalString[i][j],charBuffer,0,(struct sockaddr*)&client,clientLength);
          if (sent == -1) cerr << "Unable to send map " << i << " & " << j << "\n";

          memset(buffer, 0, bufferSize);
          received = recvfrom(socketUDP,buffer,sizeof(buffer),0,(struct sockaddr*)&client,(socklen_t*)&clientLength);
          if (received == -1) cerr << "Unable to receive from client \n";
        }
      }

      // send the propagation to client
      char* propString = new char[charBuffer];
      n = sprintf(propString,"%f",propagation);
      sent = sendto(socketUDP,propString,charBuffer,0,(struct sockaddr*)&client,clientLength);
      if (sent == -1) cerr << "Unable to send map " << i << " & " << j << "\n";
      //memset(buffer, 0, bufferSize);
      received = recvfrom(socketUDP,buffer,sizeof(buffer),0,(struct sockaddr*)&client,(socklen_t*)&clientLength);
      if (received == -1) cerr << "Unable to receive from client \n";

      // send the transmission to client
      char* transString = new char[charBuffer];
      n = sprintf(transString,"%d",transmission);
      sent = sendto(socketUDP,transString,charBuffer,0,(struct sockaddr*)&client,clientLength);
      if (sent == -1) cerr << "Unable to send map " << i << " & " << j << "\n";
      //memset(buffer, 0, bufferSize);
      received = recvfrom(socketUDP,buffer,sizeof(buffer),0,(struct sockaddr*)&client,(socklen_t*)&clientLength);
      if (received == -1) cerr << "Unable to receive from client \n";

      //sent = sendto(socketUDP,totalString,MAXNUM*sizeof(char*),0,(struct sockaddr*)&client,clientLength);
      if (sent == -1) cerr << "The server " << serverName << " unables to reply AWS with found source map\n";
      else cout << "The Server " << serverName <<  " has sent graph to AWS\n";
    }
  }
  close(socketUDP);
}

/*
  Implementation of serverC using UDP Connection

  Credit:
  Beej’s Guide to Network Programming Using Internet Sockets, Brian “Beej Jorgensen” Hall
  GeeksForGeeks UDP socket programming: https://www.geeksforgeeks.org/udp-server-client-implementation-c/
  Starter UDP Server And Client in C++: https://www.youtube.com/watch?v=uIanSvWou1M
  https://bitbucket.org/sloankelly/youtube-source-repository/src/bb84cf7f8d95d37354cf7dd0f0a57e48f393bd4b/cpp/networking/UDPClientServerBasic/UDP_Server/UDP_Server/main.cpp?at=master
  https://bitbucket.org/sloankelly/youtube-source-repository/src/bb84cf7f8d95d37354cf7dd0f0a57e48f393bd4b/cpp/networking/UDPClientServerBasic/UDP_Client/UDP_Client/main.cpp?at=master
*/
void ServerFuncs::udpServerC(string serverIPAddr, int port, string serverName){
  // Creation of a socket
  char** buffer= new char*[6];
  for (int i = 0; i < 6; i++) buffer[i] = new char[bufferSize];
  int socketUDP = socket(AF_INET,SOCK_DGRAM,0);
  if (socketUDP == -1){
    cerr << "Server" << serverName << " unables to create a socket\n";
  } else cout << "Server " << serverName << " successfully created a socket\n";

  // Bind the IP address and the port number for the socket descriptor
  struct sockaddr_in socketDescriptor;
  memset(&socketDescriptor,0,sizeof(socketDescriptor));
  inet_pton(AF_INET, serverIP.c_str(),&socketDescriptor.sin_addr.s_addr);
  socketDescriptor.sin_family = AF_INET;
  socketDescriptor.sin_port = htons(port);
  if (bind(socketUDP,(const struct sockaddr*)&socketDescriptor, sizeof(socketDescriptor)) == -1){
    cerr << "Server" << serverName << " unables to create a socket descriptor\n";
  } else cout << "Server " << serverName << " is up and running using UDP on port " << ntohs(socketDescriptor.sin_port) << "\n";

  // struct sockaddr_in sin;
  // socklen_t len = sizeof(sin);
  // if (getsockname(socketUDP, (struct sockaddr *)&sin, &len) == -1) perror("getsockname");
  // else cout << serverName << " port number " << ntohs(sin.sin_port) << "\n";

  // Receive any client
  struct sockaddr_in client;

  // Keep serverC on until Ctrl+C
  while (true){
    memset(&client,0,sizeof(client));
    int clientLength = sizeof(client);
    int received, sent, i, j;
    int charBuffer = 1024;
    char* tempStorage = "Received";
    char* propagrationString = "";

    // Receive the mapInfo from AWS
    for (i = 0; i < 7; i++){
      buffer[i] = new char[charBuffer];
      received = recvfrom(socketUDP,buffer[i],charBuffer,0,(struct sockaddr*)&client,(socklen_t*)&clientLength);
      if (received == -1) cerr << "Error in receiving bytes from client. \n";
      //else cout << "Received " << buffer[i] << "\n";

      sent = sendto(socketUDP,tempStorage,strlen(tempStorage),0,(struct sockaddr*)&client,clientLength);
      if (sent == -1) cerr << "Error in sending bytes to client\n";
    }
    cout << "Server " << serverName << " has received data for calculation: \n";
    cout << "Propagration speed: " << buffer[0] << " km/h\n";
    cout << "Transmission speed: " << buffer[1] << " KB/s\n";
    cout << "Map ID: " << buffer[2] << "\n";
    cout << "Source ID: " << buffer[3] << "  Destination ID: " << buffer[4] << "\n";
    // cout << "Map size: " << buffer[5] << "\n";
    // cout << "File size: " << buffer[6] << "\n";
    float propagration = atof(buffer[0]);
    int transmission = atof(buffer[1]);
    float fileSize = atof(buffer[6]);
    int mapSize = atoi(buffer[5]);
    int dest = atoi(buffer[4]);
    int source = atoi(buffer[3]);
    // Received the actual map from AWS
    char*** recvMapString = new char**[mapSize];
    for (i = 0; i < mapSize; i++){
      recvMapString[i] = new char*[mapSize];
      for (j = 0; j < mapSize; j++) recvMapString[i][j] = new char[charBuffer];
    }

    // Received actual map data from AWS in char* format
    for (i = 0; i < mapSize; i++){
      for (j = 0; j < mapSize; j++){
        received = recvfrom(socketUDP,recvMapString[i][j],charBuffer,0,(struct sockaddr*)&client,(socklen_t*)&clientLength);
        if (received == -1) cerr << "Unables to receive source map\n";
        //else cout << "Keep receiving from server: " << recvMapString[i][j] << "\n";

        char* tempBuffer = new char[charBuffer];
        sent = sendto(socketUDP,tempBuffer,charBuffer,0,(struct sockaddr*)&client,clientLength);
        if (sent == -1) cerr << "Unable to send input message \n";
      }
      //cout << "Received row: " << i << "\n";
    }

    // convert the char* formed map data to float-typed
    float** recvMapFloat = new float*[mapSize];
    for (i = 0; i < mapSize; i++) {
      recvMapFloat[i] = new float[mapSize];
      for (j = 0; j < mapSize; j++) recvMapFloat[i][j] = strtof(recvMapString[i][j],NULL);
    }
    // displayRow(recvMapFloat[source], mapSize);
    // displayRow(recvMapFloat[dest], mapSize);

    // Determine the shortest path
    float shortestDis = 0;
    string shortestPath = "";
    dij_algo(recvMapFloat,source,dest,&shortestDis,&shortestPath,mapSize);
    cout << "The server" << serverName << " has finished the calculation\n";
    cout << "Shortest path: " << shortestPath << "\n";
    cout << "The shortest distance: " << shortestDis << " km\n";
    float propDelay = shortestDis/propagration;
    float transDelay = fileSize/transmission;
    cout << "Transmission delay: " << transDelay << " sec \n";
    cout << "Propagration delay: " << propDelay << " sec \n";
    // Only four calculated variables to be sent back to client (AWS in this case)
    int calculatedInfoSize = 4;
    char** calculatedInfo = new char*[calculatedInfoSize];
    for (i = 0; i < calculatedInfoSize; i++) {
      if (i == 1) calculatedInfo[i] = strcpy(new char[shortestPath.length()+1],shortestPath.c_str());
      else calculatedInfo[i] = new char[charBuffer];
    }
    int n0 = sprintf(calculatedInfo[0],"%f",shortestDis); // shortest distance
    //calculatedInfo[1] = shortestPath.c_str(); // shortest path
    int n2 = sprintf(calculatedInfo[2],"%f",propDelay); // propagration delay
    int n3 = sprintf(calculatedInfo[3],"%f",transDelay); // transmission delay

    // Send the calculated map info result to AWS
    for (j = 0; j < calculatedInfoSize; j++){
      sent = sendto(socketUDP,calculatedInfo[j],charBuffer,0,(struct sockaddr*)&client,clientLength);
      if (sent == -1) cerr << "Unable to send calculated map info \n";
      //else cout << "Sent " << calculatedInfo[j] << " to client (AWS)\n";

      char* tempBuffer = new char[charBuffer];
      received = recvfrom(socketUDP,tempBuffer,charBuffer,0,(struct sockaddr*)&client,(socklen_t*)&clientLength);
      if (received == -1) cerr << "Unables to receive from client (AWS)\n";
    }
    cout << "The Server " << serverName << " has finished sending the output to AWS\n";
  }

  close(socketUDP);
}
