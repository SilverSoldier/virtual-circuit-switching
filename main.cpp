#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <algorithm>
#include "switching.h"

using namespace std;

extern vector<vector<Edge>> Topology;
extern vector<Conn> Connections;
extern int Node_count;
extern int Edge_count;
extern int Conn_count;

extern vector<vector<int>> shortest_distance;
extern vector<vector<int>> shortest_route;

extern vector<vector<int>> second_distance;
extern vector<vector<vector<int>>> second_route;

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

  // Creating adjacency matrix
  Edge edge = {INT32_MAX, 0};

  vector<Edge> init;
  init.assign(Node_count, edge);
  Topology.assign(Node_count, init);

  for(int i = 0; i < Node_count; i++){
	Topology[i][i] = {0, 0};
  }

  int from, to, delay, cap;
  for(int i = 0; i < Edge_count; i++){
	infile >> from >> to >> delay >> cap;
	edge = {delay, cap};

	Topology[from][to] = edge;
	Topology[to][from] = edge;

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
   Function to perform DFS to find the shortest path from a particular source to destination
*/
void dfs(int cur, int dest, vector<int> path, int dist, vector<int> *minPath, int *minDist){
  for(int vert = 0; vert < Node_count; vert++){
	if(vert == cur)
	  continue;
	int wt = Topology[cur][vert].delay;
	if(wt == INT32_MAX)
	  continue;

	// Reached the end of the journey - calculate stats, store if worthwhile, chuck if not
	if(vert == dest) {
	  dist += wt;
	  if(dist < *minDist){
		path.push_back(vert);
		*minPath = path;
		*minDist = dist;
	  }
	} else{
	  vector<int>::iterator it = find(path.begin(), path.end(), vert);
	  if(it == path.end()){
		path.push_back(vert);
		dfs(vert, dest, path, dist + wt, minPath, minDist);
		path.pop_back();
	  }
	}
  }
}

/**
   Function which computes all pairs 2 shortest paths.
   
   Path 1: Apply Floyd-Warshall for all pairs shortest paths.

   Path 2: Apply Pollack's algo for all pairs.
*/
void shortest_paths_hop(){
  vector<int> inf;
  inf.assign(Node_count, INT32_MAX);

  shortest_distance.assign(Node_count, inf);
  shortest_route.assign(Node_count, inf);
  second_distance.assign(Node_count, inf);
  second_route.resize(Node_count);
  
  for(int i = 0; i < Node_count; i++){
	shortest_distance[i][i] = 0;
	second_distance[i][i] = 0;
	shortest_route[i][i] = -1;
	second_route[i].resize(Node_count);
  }

  // Floyd Warshall for all pairs shortest path
  // Step 1: Initialize all edges to their weights
  for(int i = 0; i < Node_count; i++){
	for(int j = 0; j < Node_count; j++){
	  int dist = Topology[i][j].delay;
	  if(dist < INT32_MAX){
		shortest_distance[i][j] = dist;
		shortest_route[i][j] = j;
	  }
	}
  }

  // Step 2: Run Floyd Warshall algo
  for(int k = 0; k < Node_count; k++){
	for(int i = 0; i < Node_count; i++){
	  for(int j = 0; j < Node_count; j++) {
		if(i == j)
		  continue;
		if(shortest_distance[i][k] == INT32_MAX || shortest_distance[k][j] == INT32_MAX)
		  continue;

		if(shortest_distance[i][j] > shortest_distance[i][k] + shortest_distance[k][j]){
		  shortest_distance[i][j] = shortest_distance[i][k] + shortest_distance[k][j];
		  shortest_route[i][j] = shortest_route[i][k];
		}
	  }
	}
  }

  for(int i = 0; i < Node_count; i++){
	for(int j = 0; j < Node_count; j++){
	  printf("%d ", shortest_distance[i][j]);
	}
	printf("\n");
  }

  // Path 2: 2nd shortest path
  // For every link in the shortest path from u to v, change weight to INF and find next shortest path, choose min among all of them
  vector<int> minPath, argPath, tempPath;
  int minDist = INT32_MAX, tempDist = INT32_MAX;
  for(int i = 0; i < Node_count; i++){
	for(int j = 0; j < Node_count; j++){
	  if(j == i)
		continue;
	  minDist = INT32_MAX;
	  minPath.clear();
	  argPath = { i };
	  int k = i;
	  while(k != j){
		// Change the weight of edge from k to its next (along link) to INF
		int temp = Topology[k][shortest_route[k][j]].delay;
		tempDist = INT32_MAX;
		tempPath.clear();
		Topology[k][shortest_route[k][j]].delay = INT32_MAX;
		dfs(i, j, argPath, 0, &tempPath, &tempDist);
		if(tempDist < minDist){
		  minDist = tempDist;
		  minPath.assign(tempPath.begin(), tempPath.end());
		}
		// Reset change
		Topology[k][shortest_route[k][j]].delay = temp;
		k = shortest_route[k][j];
	  }
	  second_distance[i][j] = minDist;
	  second_route[i][j].assign(minPath.begin(), minPath.end());
	}
  }

  for(int i = 0; i < Node_count; i++){
	for(int j = 0; j < Node_count; j++){
	  printf("%d:", second_distance[i][j]);
	  for(auto k = second_route[i][j].cbegin(); k != second_route[i][j].cend(); k++){
		printf("%d->", *k);
	  }
	  printf("\t");
	}
	printf("\n");
  }
}

int main(int argc, char* argv[]) {
  Args args;
  parse_args(&args, argc, argv);

  // cout << args.top_file << endl;

  parse_topology(args.top_file);
  shortest_paths_hop();
}
