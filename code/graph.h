#ifndef _GRAPH_H_
#define _GRAPH_H_
#include "QA_List.h"

/** Graph Class : Deals only with integers
                  expects all nodes to be from 0,1,...n-1
                  (order of insertion doesn't matter, as long as all
                   numbers are covered before any algorithm is called)
                  NOTE: "Lone" nodes must have an edge pointing to itself
                        in order to exist within the graph!
                        This can be remedied (if needed) by simply uncommenting
                        and utilizing the add_node method, I believe.
*/

class graph {
public:
  graph();
  //void add_node(int v) {domain.insert(v);}
  void add_edge(int v, int w); // v points to w
  void get_SCC(vector<vector<int>> & SCComponents);
  string to_String();
  //bool operator==(const graph & other) const; // NOTE: Problems with const
  graph(const graph & other); // Copy constructor
  graph & operator=(graph && other) noexcept; // Move assignment operator
  graph & operator=(const graph & other); // Copy assignment operator
  ~graph();
  //-+- FOR TESTING PURPOSES ONLY:
  void DFS(int v, vector<int> & result_vec);
  void POT(int v, stack<int> & traversed_vertices);
  void reSet();
private:
  set<int> domain; // to keep track of all int's added to any container
  //void reSet();
  map<int, vector<int>> adj;
  //void DFS(int v, vector<int> & result_vec); // Depth-First Search to all REACHABLE nodes
  void DFSUtil(int v, vector<bool> & visited, vector<int> & result_vec);
  //void POT(int v, stack<int> & traversed_vertices); // Post-Order Traversal, will cover ALL vertices
  void POTUtil(int v, vector<bool> & visited, stack<int> & traversed_vertices);
  graph reverse_edges();
};

#endif
