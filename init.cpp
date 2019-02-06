#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "init.h"

using namespace std;

Args args = { "topology.txt", "connection.txt", "routingtableoutput.txt", "forwardingtable.txt", "path.txt", 0, 0 };
vector<vector<Edge>> Topology;
vector<Conn> Connections;
int Node_count;
int Edge_count;
int Conn_count;

void parse_args(Args* args, int argc, char* argv[]){
  // Checking number of arguments
  if(argc < 3) {
	// cout << "Wrong number." << endl;
	// cerr << "Usage: " << argv[0] << " -top <> -conn <> -rt <> -ft <> -path <> -flag hop|dist -p 0|1" << endl;
	// exit(1);
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
  Edge edge = {INT32_MAX, 0, 0};

  vector<Edge> init;
  init.assign(Node_count, edge);
  Topology.assign(Node_count, init);

  for(int i = 0; i < Node_count; i++){
	Topology[i][i] = {0, 0, 0};
  }

  int from, to, delay, cap;
  for(int i = 0; i < Edge_count; i++){
	infile >> from >> to >> delay >> cap;
	edge = {delay, cap, 0};

	Topology[from][to] = edge;
	Topology[to][from] = edge;

	// printf("%d->%d, %d, %d\n", from, to, delay, cap);
  }

  infile.close();
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
  Connections.push_back(conn);
  }


  infile.close();
}
