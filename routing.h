#ifndef ROUTING_H
#define ROUTING_H

#include <vector>

using namespace std;

extern vector<vector<int>> shortest_distance;
extern vector<vector<int>> shortest_route;

extern vector<vector<int>> second_distance;
extern vector<vector<vector<int>>> second_route;

int weight(int actual_weight);

void dfs(int cur, int dest, vector<int> path, int dist, vector<int> *minPath, int *minDist);

void shortest_paths();

void first_shortest_path();

void second_shortest_path();

#endif
