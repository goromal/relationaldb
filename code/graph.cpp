#include "graph.h"

graph::graph() {}

void graph::add_edge(int v, int w) {
  adj[v].insert(w);
  domain.insert(v);
  domain.insert(w);
}

bool graph::has_self_loop(int v) {
  return adj[v].find(v) != adj[v].end();
}

void graph::get_SCC(vector<vector<int>> & SCComponents) { // NOTE: Custom comparator not needed for this lab, and must be modified anyway (<<)
  /**
      This is a one-way comparison for SCC lists to determine if the left-side
      gets inserted first:

      (A) If the elements are not completely disjoint in their children:
        - There should never be any clashing, otherwise the elements would have
          been in the same SCC list anyway
      (B)) If they are all completely disjoint in every sense:
        - Ordering will be done by numerical value, which will never provide
          clashing because there are no common elements among SCC lists
  */
  /*
  auto dependency_comparator = [&] (const vector<int>& lhs, const vector<int>& rhs) -> bool {
    for (const auto& parent : lhs)
      for (const auto& child : adj[parent])
        if (std::find(rhs.begin(), rhs.end(), child) != rhs.end()) <<<<<<<<<<<<<<<<<<<<<<
          return true;
    // Case (B)
    return lhs[0] < rhs[0];
  };
  auto SCComponent_set = set<vector<int>, decltype(dependency_comparator)> (dependency_comparator);
  */
  //==========================================================================
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
    if (section_vec.size() > 0)
    {
      //SCComponent_set.insert(section_vec);
      std::sort(section_vec.begin(), section_vec.end());
      SCComponents.push_back(section_vec);
    }
    traversed_vertices.pop();
  }
  //std::copy(SCComponent_set.begin(), SCComponent_set.end(), std::back_inserter(SCComponents));
  reSet(); // NOTE: Be sure to use this function if using DFS publicly
}

void graph::DFSUtil(int v, vector<bool> & visited, vector<int> & result_vec) {
  visited[v] = true;
  if (domain.find(v) != domain.end()) { // if v is still in the domain
    result_vec.push_back(v);
    domain.erase(domain.find(v));
  }
  for (const auto& child : adj[v])
    if (!visited[child])
      DFSUtil(child, visited, result_vec);
}

void graph::DFS(int v, vector<int> & result_vec) {
  vector<bool> visited;
  for (size_t i = 0; i < domain.size(); i++) visited.push_back(false);
  DFSUtil(v, visited, result_vec);
}

void graph::POTUtil(int v, vector<bool> & visited, stack<int> & traversed_vertices) {
  if (visited[v]) return;
  visited[v] = true;
  for (const auto& child : adj[v])
    POTUtil(child, visited, traversed_vertices);
  traversed_vertices.push(v);
}

void graph::POT(int v, stack<int> & traversed_vertices) {
  vector<bool> visited;
  bool first_cycle = true;
  int cv = v;
  for (size_t i = 0; i < domain.size(); i++) visited.push_back(false); // Moved outside of loop
  while (cv != v || first_cycle) {
    POTUtil(cv, visited, traversed_vertices);
    if (size_t(cv + 1) < domain.size()) cv++;
    else cv = 0;
    first_cycle = false;
  }
}

graph graph::reverse_edges() {
  graph reversed_graph;
  for (size_t i = 0; i < domain.size(); i++) {
    reversed_graph.add_node(i);
    for (const auto& child : adj[i])
      reversed_graph.add_edge(child, i);
  }
  return reversed_graph;
}

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
  map<int, set<int>>::iterator it;
  for (it = adj.begin(); it != adj.end(); ++it) {
    domain.insert(it->first);
    for (const auto& child : it->second)
      domain.insert(child);
  }
}

string graph::to_String(map<int,string> print_map = map<int,string>()) {
  stringstream ss;
  bool map_given = (print_map.size() > 0);
  for (size_t i = 0; i < domain.size(); i++) { // Already sorted inherently
    ss << (map_given ? print_map[i] : std::to_string(i)) << ":";
    string delim = "";
    for (const auto& child : adj[i]) {
      ss << delim << (map_given ? print_map[child] : std::to_string(child));
      delim = ",";
    }
    ss << '\n';
  }
  return ss.str();
}

graph::~graph() {}
