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
#include "WeightedGraph.h"

using namespace std;

#define MAXNUM 100

// float WeightedGraph::getProp(string mapID){
//   if (propagation.count(mapID) > 0 && myGraph.count(mapID) > 0){
//     return propagation[mapID];
//   }
//   return -1;
// }

// int WeightedGraph::getTransmission(string mapID) {
//   if (transmission.count(mapID) > 0 && myGraph.count(mapID) > 0) {
//     return transmission[mapID];
//   }
//   return -1;
// }

int WeightedGraph::numOfMaps(){
  string content;
  ifstream infile;
  int num = 0;
  infile.open(fileName.c_str());
  if (!infile){
    cerr << "Unable to locate or open file " << fileName << "\n";
    exit(0);
  } else {
    while (getline(infile, content)){
      if (isalpha(content[0])) num++;
    }
    infile.close();
  }
  return num;
}

/*
  Add an edge to the map
  Parameter: source, dest, weight
*/
void WeightedGraph::addEdge(string mapID, int source, int dest, float weight){
  if (myGraph.count(mapID) < 1){
    float** tempGraph = new float*[MAXNUM];
    for (int i = 0; i < MAXNUM; i++){
      tempGraph[i] = new float[MAXNUM];
      for (int j = 0; j < MAXNUM; j++){
        if (i == source && j == dest) tempGraph[i][j] = weight;
      }
    }
    myGraph.insert(pair<string,float**>(mapID,tempGraph));
    // cout << "add ID " << mapID << " with a new 100x100 with edge from " <<
    //       source << " to " << dest << " with weight " << weight << "\n";
  } else {
    if ((myGraph.find(mapID)->second)[source][dest] == 0) {
      (myGraph.find(mapID)->second)[source][dest] = weight;
      // cout << "added an edge at ID " << mapID << " from " << source <<
      //       " to " << dest << " with weight of " << weight << " inside addEdge" << "\n";
    }
    // else cout << "Use modify function to edit path weight\n";
  }
}

void WeightedGraph::editEdge(string mapID, int source, int dest, float newWeight){
  if (myGraph.count(mapID) > 0){
    cout << "The old weight from " << source << " to " << dest << " was " <<
          (myGraph.find(mapID)->second)[source][dest] << "\n";
    (myGraph.find(mapID)->second)[source][dest] = newWeight;
  }
}

void WeightedGraph::setProp(string mapID, float input){
  if (myGraph.count(mapID) < 1) propagation.insert(pair<string,float>(mapID,input));
}

void WeightedGraph::setTransmission(string mapID, int input){
  if (myGraph.count(mapID) < 1) transmission.insert(pair<string,int>(mapID,input));
}

/*
  Display the existing map on the terminal screen
*/
void WeightedGraph::display(string mapID){
  if (myGraph.count(mapID) > 0){
    cout << "[";
    for (int i = 0; i < (sizeof(myGraph[mapID])/ sizeof(myGraph[mapID][0])); i++){
      cout << "i = "<<i<<"\n";
      cout << "[";
      for (int j = 0; j < (sizeof(myGraph[mapID][i])/sizeof(int)); j++){
        if (j == sizeof myGraph[mapID][i] - 1) cout << (myGraph.find(mapID)->second)[i][j];
        else cout << (myGraph.find(mapID)->second)[i][j] << " ";
      }
      cout << "]";
    }
    cout << "]\n";
  }
}

void WeightedGraph::loadMap(string inputFile){
  string content, tempID;
  ifstream infile;
  int counter = 0;
  infile.open(inputFile.c_str());
  if (!infile){
    cerr << "Unable to locate or open file " << inputFile << "\n";
    exit(0);
  } else {
    while (getline(infile, content)){
      // A new map if the getline is an alphabet
      if (isalpha(content[0])) {
        tempID = content;
        counter = 0;
        //cout << "A new mapID " << content << "\n";
      } else if ((isdigit(content[0]) && isdigit(content[1]) && !isdigit(content[2])) ||
                (isdigit(content[0]) && !isdigit(content[1]) && isdigit(content[2]))){
        // Insert the edge from source to destination with weight
        char charArray[content.length()+1];
        strcpy(charArray, content.c_str());
        char *line = strtok(charArray," ");
        int source = 0, dest = 0, whileCounter = 0;
        float weight = 0.0;
        while (line != NULL && whileCounter < 3){
          if (whileCounter == 0) {
            int s = 0;
            sscanf(line,"%d",&s);
            source = s;
          }
          else if (whileCounter == 1) {
            int d = 0;
            sscanf(line,"%d",&d);
            dest = d;
          }
          else {
            float w = 0.0;
            sscanf(line,"%f",&w);
            weight = w;
          }
          line = strtok(NULL, " ");
          whileCounter++;
        }
        addEdge(tempID, source,dest,weight);
      }
      if (counter == 1){
        // Get the propagation speed
        float x = 0.0;
        sscanf(content.c_str(),"%f",&x);
        setProp(tempID,x);
        // cout << "Set propagation at ID " << tempID << "\n";
      }
      if (counter == 2){
        // Get the transmission speed
        int y = 0;
        sscanf(content.c_str(),"%d",&y);
        setTransmission(tempID,y);
        // cout << "Set transmission at ID " << tempID << "\n";
      }
      counter++;
    }

  }
  //cout << "Map is loaded\n";
}
