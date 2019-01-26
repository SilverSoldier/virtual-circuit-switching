#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "switching.h"

using namespace std;

extern vector<vector<Edge>> Topology;
extern vector<Conn> Connections;
extern int Node_count;
extern int Edge_count;
extern int Conn_count;

extern Pair **shortest_distance;
extern Pair **shortest_route;

void parse_args(Args* args, int argc, char* argv[]){
  // Checking number of arguments
  if(argc < 3) {
	// cout << "Wrong number." << endl;
	cerr << "Usage: " << argv[0] << " -top <> -conn <> -rt <> -ft <> -path <> -flag hop|dist -p 0|1" << endl;
	exit(1);
  }
  for(int i = 1; i < argc; i += 2){
	string arg_oi = string(argv[i]);
	if(arg_oi == "-top"){
	  args->top_file = argv[i+1];
	  // cout << "Parsed top file" << endl;
	} else if(arg_oi == "-conn"){
	  args->conn_file = argv[i+1];
	  // cout << "Parsed conn file" << endl;
	} else if(arg_oi == "-rt") {
	  args->rt_file = argv[i+1];
	} else if(arg_oi == "-ft") {
	  args->ft_file = argv[i+1];
	} else if(arg_oi == "-path") {
	  args->path_file = argv[i+1];
	} else if(arg_oi == "-flag") {
	  string flag_arg = string(argv[i+1]);
	  if(flag_arg == "hop")
		args->flag = 0;
	  else if(flag_arg == "dist")
		args->flag = 1;
	  else {
		cerr << "Usage: " << argv[0] << " -top <> -conn <> -rt <> -ft <> -path <> -flag hop|dist -p 0|1";
		exit(1);
	  }
	} else if(arg_oi == "-p") {
	  args->pessimistic = (argv[i+1][0] == '1');
	} else {
	  // cout << "Trouble with: " << arg_oi << endl;
	  cerr << "Usage: " << argv[0] << " -top <> -conn <> -rt <> -ft <> -path <> -flag hop|dist -p 0|1";
	  exit(1);
	}
  }
}

/**
Function to parse the topology information: edges along with their delay (weight) and capacity

Store the information in an adjacency list. Since bi-directional store for both the nodes.
 */
void parse_topology(char* file_name) {
  ifstream infile(file_name);
  
  infile >> Node_count >> Edge_count;

  // Creating adjacency list
  Topology.reserve(Node_count);

  Edge edge_fwd, edge_bwd;
  for(int i = 0; i < Edge_count; i++){
	int from, to, delay, cap;
	infile >> from >> to >> delay >> cap;
	edge_fwd = {to, delay, cap};
	edge_bwd = {from, delay, cap};

	Topology[from].push_back(edge_fwd);
	Topology[to].push_back(edge_bwd);

	// printf("%d->%d, %d, %d\n", from, to, delay, cap);
  }
}

/**
   Function to parse the connections file.

Connections are stored in the vector connections.
 */
void parse_connections(char* file_name) {
  ifstream infile(file_name);

  infile >> Conn_count;
    Conn conn;

  for(int i = 0; i < Conn_count; i++) {
	infile >> conn.source >> conn.dest >> conn.min >> conn.avg >> conn.max;
  }
}

/**
   Function which computes all pairs 2 shortest paths.
   
   Path 1: Apply Floyd-Warshall for all pairs shortest paths.

   Path 2: Apply Pollack's algo for all pairs.
 */
void shortest_paths_hop(){
  shortest_distance = (Pair**) malloc(Node_count * sizeof(Pair*));
  shortest_route = (Pair**) malloc(Node_count * sizeof(Pair*));
  for(int i = 0; i < Node_count; i++){
	// Each element is a Pair array of size Node_count
	shortest_distance[i] = (Pair*) malloc(Node_count * sizeof(Pair));
	shortest_route[i] = (Pair*) malloc(Node_count * sizeof(Pair));
	for(int j = 0; j < Node_count; j++){
	  // Set all distances to infinity initially
	  shortest_distance[i][j][0] = INT32_MAX;
	  shortest_distance[i][j][1] = INT32_MAX;
	  shortest_route[i][j][0] = INT32_MAX;
	  shortest_route[i][j][1] = INT32_MAX;
	}
	shortest_distance[i][i][0] = 0;
	shortest_distance[i][i][1] = 0;
  }

  // Floyd Warshall for all pairs shortest path
  // Step 1: Initialize all edges to their weights
  for(int i = 0; i < Node_count; i++){
	for(Edge edge: Topology[i]){
	  shortest_distance[i][edge.dest][0] = edge.delay;
	  shortest_route[i][edge.dest][0] = edge.dest;
	}
  }

  for(int k = 0; k < Node_count; k++){
	for(int i = 0; i < Node_count; i++){
	  for(int j = 0; j < Node_count; j++) {
		if(shortest_distance[i][j][0] > shortest_distance[i][k][0] + shortest_distance[k][j][0]){
		  shortest_distance[i][j][0] = shortest_distance[i][k][0] + shortest_distance[k][j][0];
		  shortest_route[i][j][0] = shortest_route[i][k][0];
		}
	  }
	}
  }

  for(int i = 0; i < Node_count; i++){
	for(int j = 0; j < Node_count; j++){
	  printf("%d ", shortest_distance[i][j][0]);
	}
	printf("\n");
  }

  // Path 2: 2nd shortest path
  // For every link in the shortest path from u to v, change weight to INF and find next shortest path
  // for(int i = 0; i < Node_count; i++){
  // 	for(int j = 0; j < Node_count; j++){
  // 	}
  // }
}

int main(int argc, char* argv[]) {
  Args args;
  parse_args(&args, argc, argv);

  // cout << args.top_file << endl;

  parse_topology(args.top_file);
  shortest_paths_hop();
}
