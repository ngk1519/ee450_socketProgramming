// Functions utilities files
// By Kevin Ng

#include <cstring>
#include <string.h>
#include <map>
#pragma once

using namespace std;

#define MAXNUM 100

class WeightedGraph{
  private:
    int edge;
    string fileName;
    map<string,float> propagation;
    map<string,int> transmission;
    map<string,float**> myGraph;

  public:
    WeightedGraph(string inputFile){
      cout << "WeightedGraph object is created\n";
      edge = 0;
      fileName = inputFile;
      loadMap(inputFile);
    };
    void addEdge(string mapID, int source, int dest, float weight);
    void editEdge(string mapID, int source, int dest, float newWeight);
    void setFileName(string inputFile) { fileName = inputFile; };
    void setProp(string mapID, float input);
    void setTransmission(string mapID, int input);
    void display(string mapID);
    void loadMap(string inputFile);
    float getProp(string mapID){
      if (propagation.count(mapID) > 0 && myGraph.count(mapID) > 0){
        return propagation[mapID];
      }
      return 0;
    };
    int getTransmission(string mapID){
      if (transmission.count(mapID) > 0 && myGraph.count(mapID) > 0) {
        return transmission[mapID];
      }
      return 0;
    };
    bool containsMap(string mapID) { return (myGraph.count(mapID) > 0); };
    // bool containsSource(string mapID, int source)
    float** getMap(string mapID){
      if (myGraph.count(mapID) > 0) return (myGraph.find(mapID))->second;
      return {0};
    };
    float* getMapSource(string mapID, int source){
      if (myGraph.count(mapID) > 0) return (myGraph.find(mapID))->second[source];
      return {0};
    }
    int numEdge(string mapID){
      int count = 0;
      if (myGraph.count(mapID) > 0){
        for (int i = 0; i < MAXNUM; i++){
          for (int j = 0; j < MAXNUM; j++){
            if (((myGraph.find(mapID))->second)[i][j] != 0) count++;
          }
        }
      }
      return count;
    };
    // Modify numVertices
    // int numVertices(string mapID) { return sizeof myGraph/ sizeof myGraph[0]; };
    bool hasEdge(string mapID, int source, int dest){
      if (myGraph.count(mapID) > 0) return (myGraph.find(mapID)->second)[source][dest] != 0;
      else return false;
    };
    string getFileName() { return fileName; }
    int numOfMaps();
};
