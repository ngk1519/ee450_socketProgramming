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
#define SERVERBPORT 31519
// IP address for the local machine
const string serverIP = "127.0.0.1";

// The map txt file must be stored inside the directory of "maps" inside of the project directory.
const string fileName2 = "maps/map2.txt";
const string serverName = "B";

int main(){
  WeightedGraph map2(fileName2);
  //displayRow(maps.getMap("i")[49]);
  ServerFuncs serverB("serverB");
  serverB.udpServer(serverIP, SERVERBPORT, serverName, map2);

  return 0;
}
