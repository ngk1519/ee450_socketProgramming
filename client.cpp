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
#include "ServerFuncs.h"

using namespace std;

// Pre-set server port number, determined on the server side
#define AWSPORT 34519

// IP address for the local machine
const string serverIP = "127.0.0.1";
//const int bufferSize = 8192;

// Global varibles for storing the desired input
// string mapID = "";
// int sourceVertex = 0;
// int destVertex = 0;
// int fileSize = 0;

int main(int argc, char* argv[]){
  ServerFuncs clientRequest("client");
  clientRequest.tcpClient(serverIP,AWSPORT,argc,argv);

  return 0;
}
