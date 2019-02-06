#include <stdio.h>
#include <iostream>
#include "init.h"
#include "routing.h"
#include "connection.h"

/**
Maintain a allocation list for each router.
Parse all connections in Connections.
For every link, verify if capacity can be met.
If yes, reduce capacity in every link, assign VCId to every link.
*/

vector<Router> routers;

void allocate(){
  routers.resize(Node_count);
  for(int i = 0; i < Conn_count; i++){
	Conn *conn = &Connections[i];
	// Checking the shortest path from conn.source to conn.dest
	int k = conn->source;

	// Calculate b_equiv for that link
	int b_equiv = min(conn->max, int(conn->avg  + 0.25 * (conn->max - conn->min)));
	bool fail = false;
	// Check if all paths have sufficient capacity
	while(k != conn->dest){
	  int next = shortest_route[k][conn->dest];
	  if(Topology[k][next].capacity < b_equiv)
		fail = true;
	  k = next;
	}

	// If sufficient capacity available, allocate VCIds on the path
	if(!fail){
		int b_equiv = min(conn->max, int(conn->avg  + 0.25 * (conn->max - conn->min)));
		int cur = shortest_route[conn->source][conn->dest];
		int prev = conn->source;
		int next = shortest_route[cur][conn->dest];
		conn->cost = shortest_distance[conn->source][conn->dest];
		// Reduce capacity of prev link
		Topology[prev][cur].capacity -= b_equiv;
		int prev_vcid = Topology[conn->source][cur].last_vcid++;
		routers[prev].links.push_back({-1, -1, cur, prev_vcid});
		conn->vcid.append(to_string(prev_vcid));
		conn->vcid.push_back(',');
		while(cur != conn->dest){
		  conn->path.append(to_string(cur));
		  conn->path.push_back(',');
		  // Reduce capacity of link
		  Topology[cur][next].capacity -= b_equiv;
		  int vcid = Topology[cur][next].last_vcid++;
		  routers[cur].links.push_back({prev, prev_vcid, next, vcid});
		  conn->vcid.append(to_string(vcid));
		  conn->vcid.push_back(',');
		  // Store the previous link details
		  prev = cur;
		  cur = next;
		  next = shortest_route[prev][conn->dest];
		  prev_vcid = vcid;
		}
		routers[conn->dest].links.push_back({prev, prev_vcid, -1, -1});
	}
  }

  // Print routerwise information
  cout << "Router information\n";
  cout << "ID\tIn Id\tVCID\tOut Id\tVCID\n";
  for(int i = 0; i < Node_count; i++){
	for(Link link: routers[i].links){
	  cout << i << "\t";
	  cout << link.i_router << "\t";
	  cout << link.i_vcid << "\t";
	  cout << link.o_router << "\t";
	  cout << link.o_vcid << "\n";
	}
	cout << "\n";
  }

  // Print connection wise information
  cout << "Connections information\n";
  cout << "Id\tSource\tDest\tPath\t\tVCID List\tPath Cost\n";
  for(int i = 0; i < Conn_count; i++){
	Conn conn = Connections[i];
	cout << i << "\t";
	cout << conn.source << "\t";
	cout << conn.dest << "\t";
	cout << conn.path << "\t\t";
	cout << conn.vcid << "\t\t";
	cout << conn.cost << "\n";
  }
}
