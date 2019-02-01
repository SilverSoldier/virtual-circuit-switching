#ifndef SWITCHING_H
#define SWITCHING_H

#include <vector>

using namespace std;

typedef struct arguments {
  char* top_file;
  char* conn_file;
  char* rt_file;
  char* ft_file;
  char* path_file;
  bool flag;
  bool pessimistic;
} Args;

typedef struct edge {
  int delay;
  int capacity;
} Edge;

typedef struct conn {
  int source;
  int dest;
  int min;
  int avg;
  int max;
} Conn;

vector<vector<Edge>> Topology;
vector<Conn> Connections;
int Node_count;
int Edge_count;
int Conn_count;

// The pair-wise shortest paths
vector<vector<int>> shortest_distance;
vector<vector<int>> shortest_route;

vector<vector<int>> second_distance;
vector<vector<vector<int>>> second_route;

#endif
