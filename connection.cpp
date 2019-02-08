#include <stdio.h>
#include <iostream>
#include <fstream>
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

int calculate_bequiv(int bmax, int bmin, int bavg){
  if(args.pessimistic == 1)
	return bmax;
  else
	return min(bmax, int(bavg  + 0.25 * (bmax - bmin)));
}

void allocate(){
  routers.resize(Node_count);
  for(int i = 0; i < Conn_count; i++){
	Conn *conn = &Connections[i];
	// Checking the shortest path from conn.source to conn.dest
	int k = conn->source;
	int b_equiv = calculate_bequiv(conn->max, conn->min, conn->avg);

	// Calculate b_equiv for that link
	bool fail = false;
	// Check if all paths have sufficient capacity
	while(k != conn->dest){
	  int next = shortest_route[k][conn->dest];
	  if(Topology[k][next].capacity < b_equiv){
		fail = true;
		break;
	  }
	  k = next;
	}

	// If sufficient capacity available, allocate VCIds on the path
	if(!fail){
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
	// Sufficient capacity not available on shortest path, trying second shortest path
	else {
	  fail = false;
	  // Check if all links in path have sufficient capacity
	  int prev = conn->source, next, i = 1;
	  while(i != second_route[conn->source][conn->dest].size()){
		next = second_route[prev][conn->dest][i];
		if(Topology[prev][next].capacity < b_equiv){
		  fail = true;
		  break;
		}
		prev = next;
		i++;
	  }

	  // Reduce capacity on all links and allocate VCIDs
	  if(!fail){
		int i = 1;
		int cur = second_route[conn->source][conn->dest][1];
		int prev = conn->source, next;
		conn->cost = second_distance[conn->source][conn->dest];
		// Reduce capacity of prev link
		Topology[prev][cur].capacity -= b_equiv;
		int prev_vcid = Topology[conn->source][cur].last_vcid++, vcid;
		routers[prev].links.push_back({-1, -1, cur, prev_vcid});
		conn->vcid.append(to_string(prev_vcid));
		conn->vcid.push_back(',');
		while(i != second_route[conn->source][conn->dest].size() - 1){
		  next = second_route[conn->source][conn->dest][i+1];
		  conn->path.append(to_string(cur));
		  conn->path.push_back(',');
		  // Reduce capacity of link
		  Topology[cur][next].capacity -= b_equiv;
		  vcid = Topology[cur][next].last_vcid++;
		  routers[cur].links.push_back({prev, prev_vcid, next, vcid});
		  conn->vcid.append(to_string(vcid));
		  conn->vcid.push_back(',');
		  // Store the previous link details
		  prev = cur;
		  cur = next;
		  prev_vcid = vcid;
		  i++;
		}
		routers[conn->dest].links.push_back({prev, prev_vcid, -1, -1});
	  }
	}
  }

  ofstream router_file(args.ft_file);
  // Print routerwise information
  router_file << "Router information\n";
  router_file << "ID\tIn Id\tVCID\tOut Id\tVCID\n";
  for(int i = 0; i < Node_count; i++){
	for(Link link: routers[i].links){
	  router_file << i << "\t";
	  router_file << link.i_router << "\t\t";
	  router_file << link.i_vcid << "\t\t";
	  router_file << link.o_router << "\t\t";
	  router_file << link.o_vcid << "\n";
	}
	router_file << "\n";
  }

  router_file.close();

  ofstream conn_file(args.path_file);

  // Print connection wise information
  conn_file << "Id\tSource\tDest\tPath\t\tVCID List\tPath Cost\n";
  for(int i = 0; i < Conn_count; i++){
	Conn conn = Connections[i];
	conn_file << i << "\t";
	conn_file << conn.source << "\t";
	conn_file << conn.dest << "\t";
	conn_file << conn.path << "\t\t";
	conn_file << conn.vcid << "\t\t";
	conn_file << conn.cost << "\n";
  }
}
