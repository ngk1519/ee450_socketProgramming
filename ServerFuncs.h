#include <cstring>
#include <string.h>
#include <map>
#include "WeightedGraph.h"

#define MAXNUM 100

using namespace std;

class ServerFuncs{
  public:
    ServerFuncs(string inputName){
      name = inputName;
    };

    void displayRow(float inputArray[], int size);

    void awsServer(string serverIPAddr, int port, int portA, int portB, int portC, int port2, float** resultMap, string serverName);
    void udpServer(string serverIPAddr, int port, string serverName, WeightedGraph maps);
    void udpServerC(string serverIPAddr, int port, string serverName);
    void udpClientToC(string serverIPAddr, int port, int port2, char* mapInfo[], float** resultMap, char**& calculatedResult);
    void tcpClient(string serverIPAddr, int port, int argc, char* argv[]);
    void udpClient(string serverIPAddr, int port, int port2, int argc, char* argv[], bool* found, float**& resultMap, char*& prop, char*& trans,string serverName);

  private:
    string name;
};
