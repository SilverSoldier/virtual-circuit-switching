#include <stdio.h>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include "routing.h"
#include "init.h"

vector<vector<int>> shortest_distance;
vector<vector<int>> shortest_route;

vector<vector<int>> second_distance;
vector<vector<vector<int>>> second_route;

int weight(int actual_weight){
  return (args.flag ? actual_weight : 1);
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
	  dist += weight(wt);
	  if(dist < *minDist){
		path.push_back(vert);
		*minPath = path;
		*minDist = dist;
	  }
	} else{
	  vector<int>::iterator it = find(path.begin(), path.end(), vert);
	  if(it == path.end()){
		path.push_back(vert);
		dfs(vert, dest, path, dist + weight(wt), minPath, minDist);
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
void shortest_paths(){
  first_shortest_path();
  second_shortest_path();
  // Write the routing information to routetablefile
  ofstream file(args.rt_file);

  file << "Shortest paths\n";
  for(int i = 0; i < Node_count; i++){
	file << "Node: " << i << endl;
	file << "Dest\t\t" << "Path (from Source to Dest)\t" << "Delay\n";
	for(int j = 0; j < Node_count; j++){
	  if(j != i){
		file << j << "\t\t";
		int k = i;
		while(k != -1){
		  file << k << ",";
		  k = shortest_route[k][j];
		}
		file << "\t\t\t";
		file << shortest_distance[i][j];
		file << endl;
	  }
	}
  }
  file << "\n\n";

  file << "Second shortest paths\n";
  for(int i = 0; i < Node_count; i++){
	file << "Node: " << i << endl;
	file << "Dest\t" << "Path (from Source to Dest)\t" << "Delay\n";
	for(int j = 0; j < Node_count; j++){
	  if(j != i){
		file << j << "\t";
		for(auto k: second_route[i][j]){
		  file << k << ",";
		}
		file << "\t\t";
		file << second_distance[i][j] << endl;
	  }
	}
	file << "\n";
  }

  file << "\n\n";
  
  file.close();
}

void first_shortest_path(){
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
	  if(i == j)
		continue;
	  int wt = Topology[i][j].delay;
	  if(wt < INT32_MAX){
		shortest_distance[i][j] = weight(wt);
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
}

void second_shortest_path() {
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

