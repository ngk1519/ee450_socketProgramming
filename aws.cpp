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
#define SERVERAPORT 30519
#define SERVERBPORT 31519
#define SERVERCPORT 32519
#define AWSCLIENTPORT 33519
// Number of pending connection allows to be queued up
#define QUEUESIZE 256
#define MAXNUM 100

// IP address for the local machine
const string serverIP = "127.0.0.1";
const string serverName = "AWS";

int main(){
  float** resultMap = new float*[MAXNUM];
  for (int i = 0; i < MAXNUM; i++) resultMap[i] = new float[MAXNUM];
  ServerFuncs awsServer("AWS server");
  awsServer.awsServer(serverIP,AWSPORT,SERVERAPORT,SERVERBPORT,SERVERCPORT,AWSCLIENTPORT,resultMap,serverName);

  return 0;
}
