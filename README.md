# Virtual Circuit Switching

Lab assignment to implement virtual circuit switching.

## Files
1. `main.cpp`: main driver program
2. `init.cpp`: for reading inputs from files and setting up global variables etc.
3. `routing.cpp`: for parsing the topology and setting up the routing table and forwarding table
4. `connection.cpp`: for setting up virtual circuits for all the connection requests.

## Inputs and Outputs
Program takes as input:
1. A topology in the form of N bi-directional links with a propagation delay and a link capacity 
2. A set of connection requests in the form of source, destination, max, avg and min bandwidth.

Program generates:
1. Routing table and forwarding table for each node (by running a shortest path algorithm)
2. Connection-wise path by setting up link-unique VCIDs for every connection along all links of the path which is written to `path.txt`

### Compilation and Execution of Program
Run
```
make
```
to compile.

Run
```
./main -top topology.txt -conn connection.txt -rt routingtable.txt -ft forwardingtable.txt -path path.txt -flag hop -p 0
```
to execute
