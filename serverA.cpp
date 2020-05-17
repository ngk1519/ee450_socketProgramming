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
#include "WeightedGraph.h"
#include "ServerFuncs.h"

using namespace std;

// Pre-set server port number, determined on the server side
#define SERVERAPORT 30519
// IP address for the local machine
const string serverIP = "127.0.0.1";
// // Client IP address size
// const int clientInfoSize = 256;
// // Buffer size for storing client data
// const int bufferSize = 8192;

// The map txt file must be stored inside the directory of "maps" inside of the project directory.
const string fileName1 = "maps/map1.txt";
const string serverName = "A";

int main(){
  WeightedGraph map1(fileName1);
  //displayRow(maps.getMap("i")[49]);
  ServerFuncs serverA("serverA");
  serverA.udpServer(serverIP, SERVERAPORT, serverName, map1);

  return 0;
}
