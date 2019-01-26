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
  int dest;
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

typedef int Pair[2];

// The pair-wise shortest paths
Pair **shortest_distance;
Pair **shortest_route;

#endif
