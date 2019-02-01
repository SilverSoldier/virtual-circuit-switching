#ifndef INIT_H
#define INIT_H

#include <vector>

using namespace std;

typedef struct arguments {
  char* top_file;
  char* conn_file;
  char* rt_file;
  char* ft_file;
  char* path_file;
  bool flag; // Hop = 0, Dist = 1
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

extern Args args;

extern vector<vector<Edge>> Topology;
extern vector<Conn> Connections;
extern int Node_count;
extern int Edge_count;
extern int Conn_count;

void parse_args(Args* args, int argc, char* argv[]);

void parse_topology(char* file_name);

void parse_connections(char* file_name);

#endif
