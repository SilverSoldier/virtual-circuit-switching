#ifndef CONNECTION_H
#define CONNECTION_H

#include <vector>

using namespace std;
typedef struct Conn_link {
  int i_router;
  int i_vcid;
  int o_router;
  int o_vcid;
} Link;

typedef struct Router {
  vector<Link> links;
} Router;

void allocate();

#endif
