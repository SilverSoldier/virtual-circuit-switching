#include "main.h"
#include "init.h"
#include "routing.h"
#include "connection.h"

using namespace std;

int main(int argc, char* argv[]) {
  parse_args(&args, argc, argv);
  parse_topology(args.top_file);
  parse_connections(args.conn_file);
  shortest_paths();
  allocate();
}
