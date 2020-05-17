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

#define AWSPORT 34519
#define SERVERCPORT 32519

// IP address for the local machine
const string serverIP = "127.0.0.1";
const string serverName = "C";

int main(){
  ServerFuncs serverC("serverC");
  serverC.udpServerC(serverIP,SERVERCPORT,serverName);
  return 0;
}
