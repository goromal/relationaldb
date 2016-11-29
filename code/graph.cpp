#include "graph.h"

graph::graph() {}

void graph::add_edge(int v, int w) {
  adj[v].push_back(w);
  domain.insert(v);
  domain.insert(w);
}

void graph::get_SCC(vector<vector<int>> & SCComponents) {
  vector<int> exclude_these;
  // (1.) Reverse edges
  graph rg = this->reverse_edges();
  // (2.) Do post-order traversal of reversed graph
  stack<int> traversed_vertices;
  rg.POT(0, traversed_vertices);
  // (3.) Do depth-first search of original graph using stack
  while (traversed_vertices.size() > 0 && domain.size() > 0) {
    vector<int> section_vec;
    DFS(traversed_vertices.top(), section_vec);
    if (section_vec.size() > 0) SCComponents.push_back(section_vec);
    traversed_vertices.pop();
  }
  reSet(); // NOTE: Be sure to use this function if using DFS publicly
}

void graph::DFSUtil(int v, vector<bool> & visited, vector<int> & result_vec) {
  visited[v] = true;
  if (domain.find(v) != domain.end()) { // if v is still in the domain
    result_vec.push_back(v);
    domain.erase(domain.find(v));
  }
  for (size_t i = 0; i < adj[v].size(); i++)
    if (!visited[adj[v][i]])
      DFSUtil(adj[v][i], visited, result_vec);
}

void graph::DFS(int v, vector<int> & result_vec) {
  /*
  //----------------------------------------------------------------------------
  cout << "ENTERED DFS for " << v << " : ";//-----------------------------------
  set<int>::iterator it;
  for (it = domain.begin(); it != domain.end(); ++it)
    cout << *it << " ";
  cout << endl;//---------------------------------------------------------------
  //----------------------------------------------------------------------------
  */
  vector<bool> visited;
  for (size_t i = 0; i < domain.size(); i++) visited.push_back(false);
  DFSUtil(v, visited, result_vec);
}

void graph::POTUtil(int v, vector<bool> & visited, stack<int> & traversed_vertices) {
  if (visited[v]) return;
  visited[v] = true;
  for (size_t i = 0; i < adj[v].size(); i++)
    POTUtil(adj[v][i], visited, traversed_vertices);
  traversed_vertices.push(v);
  //cout << v << endl; //---------------------------------------------------------
}

void graph::POT(int v, stack<int> & traversed_vertices) {
  vector<bool> visited;
  bool first_cycle = true;
  int cv = v;
  while (cv != v || first_cycle) {
    for (size_t i = 0; i < domain.size(); i++) visited.push_back(false);
    POTUtil(cv, visited, traversed_vertices);
    if (size_t(cv + 1) < domain.size()) cv++;
    else cv = 0;
    first_cycle = false;
  }
}

graph graph::reverse_edges() {
  graph reversed_graph;
  map<int, vector<int>>::iterator it;
  for (it = adj.begin(); it != adj.end(); ++it)
    for (size_t i = 0; i < it->second.size(); i++)
      reversed_graph.add_edge(it->second[i], it->first);
  return reversed_graph;
}
/*
bool graph::operator==(const graph & other) const {
  if (adj.size() != other.adj.size()) return false;
  map<int, vector<int>>::const_iterator it;
  for (it = other.adj.begin(); it != other.adj.end(); ++it) {
    if (adj.find(it->first) == adj.end()) return false;
    if (adj[it->first] != it->second) return false;
  }
  return true;
}
*/
graph::graph(const graph & other) {
  adj = other.adj;
  domain = other.domain;
}

graph & graph::operator=(graph && other) noexcept {
  adj.clear();
  domain.clear();
  adj = other.adj;
  domain = other.domain;
  other.adj.clear();
  other.domain.clear();
  return *this;
}

graph & graph::operator=(const graph & other) {
  graph tmp(other);
  *this = std::move(tmp);
  return *this;
}

void graph::reSet() {
  map<int, vector<int>>::iterator it;
  for (it = adj.begin(); it != adj.end(); ++it) {
    domain.insert(it->first);
    for (size_t i = 0; i < it->second.size(); i++)
      domain.insert(it->second[i]);
  }
}

string graph::to_String() {
  stringstream ss;
  for (size_t i = 0; i < adj.size(); i++) {
    ss << i << " -> ";
    for (size_t j = 0; j < adj[i].size(); j++)
      ss << adj[i][j] << " ";
    ss << "\n";
  }
  return ss.str();
}

graph::~graph() {}
